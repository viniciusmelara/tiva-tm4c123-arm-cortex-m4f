#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "functions.h"

void ConfigUART0(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioConfig(0, 115200, SysCtlClockGet());
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
  UARTEnable(UART0_BASE);
}

void ConfigI2C3(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  GPIOPinConfigure(GPIO_PD0_I2C3SCL);
  GPIOPinConfigure(GPIO_PD1_I2C3SDA);
  GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);
  GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);
  I2CMasterInitExpClk(I2C3_BASE, SysCtlClockGet(), false);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_6);
  GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_7);
  GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD_WPU);
  GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD_WPU);
}

void ConfigLedRGB(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED | BLUE | GREEN); // LED RGB
}

uint32_t UARTDecGet(const uint32_t uartx_base)
{
  uint32_t num = 0, size = 0;

  uint8_t chr = UARTCharGet(uartx_base);
  while (chr != 0x0D) // 0x0D -> Enter / CR
  {
    if ((chr >= '0') && (chr <= '9'))
    {
      num = 10 * num + (chr - 0x30); // (chr - 48) -> decimal representation of hex
      size++;
      UARTCharPut(uartx_base, chr);
    }
    else if ((chr == 0x08) && size) // Backspace
    {
      num /= 10;
      size--;
      UARTCharPut(uartx_base, chr);
      UARTprintf("\x1B[J");
    }

    chr = UARTCharGet(uartx_base);
  }

  return num;
}

uint8_t Dec2BCD(uint8_t val)
{
  return ((val / 10) << 4) | (val % 10);
}

uint8_t BCD2Dec(uint8_t val)
{
  return (((val & 0xF0) >> 4) * 10) + (val & 0x0F);
}

void SetRTC(const uint8_t mode, const uint8_t sec_day, const uint8_t min_month, const uint8_t hour_year)
{
  uint8_t vet[3], start_addr = 0;
  vet[0] = Dec2BCD(sec_day);
  vet[1] = Dec2BCD(min_month);
  vet[2] = Dec2BCD(hour_year);

  start_addr = mode >= 1 ? DAY : SEC;

  I2CMasterSlaveAddrSet(I2C3_BASE, RTC, false);                 // R/S = 0 (Send)
  I2CMasterDataPut(I2C3_BASE, start_addr);                      // Initial address
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Start
  while (I2CMasterBusy(I2C3_BASE))
    ;

  for (register uint8_t i = 0; i < 3; i++)
  {
    I2CMasterDataPut(I2C3_BASE, vet[i]);
    I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_CONT); // Send
    while (I2CMasterBusy(I2C3_BASE))
      ;
  }

  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // Stop
  while (I2CMasterBusy(I2C3_BASE))
    ;
}

uint8_t GetRTC(const uint8_t addr)
{
  I2CMasterSlaveAddrSet(I2C3_BASE, RTC, false); // R/S = 0 (Send)
  I2CMasterDataPut(I2C3_BASE, addr);            // Address DS1307
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_SEND);
  while (I2CMasterBusy(I2C3_BASE))
    ;

  I2CMasterSlaveAddrSet(I2C3_BASE, RTC, true);                // R/S = 1 (Read)
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE); // Receive
  while (I2CMasterBusy(I2C3_BASE))
    ;

  uint8_t data = I2CMasterDataGet(I2C3_BASE);

  return BCD2Dec(data);
}

uint16_t GetHTU(const uint8_t addr)
{
  uint8_t read[2];

  I2CMasterSlaveAddrSet(I2C3_BASE, HTU, false); // R/S=0 (Send)

  I2CMasterDataPut(I2C3_BASE, addr); // Trigger Temperature or RH
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_SEND);
  while (I2CMasterBusy(I2C3_BASE))
    ;

  I2CMasterSlaveAddrSet(I2C3_BASE, HTU, true);                  // R/S=1 (Read)
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Start
  while (I2CMasterBusy(I2C3_BASE))
    ;

  read[1] = I2CMasterDataGet(I2C3_BASE);                       // Read MSB (8 bits data)
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_CONT); // Continue
  while (I2CMasterBusy(I2C3_BASE))
    ;

  read[0] = I2CMasterDataGet(I2C3_BASE);                         // Read LSB (6 bits data + 2 status bits)
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // Finish
  while (I2CMasterBusy(I2C3_BASE))
    ;

  uint16_t data = (read[1] << 8) + read[0]; // 14 data bits + 2 status bits
                                            // Status -> 00 = temp, 10 = RH
  return data;
}

void WriteExternalEEPROM(const uint8_t addr_h, const uint8_t addr_l, const uint8_t data)
{
  I2CMasterSlaveAddrSet(I2C3_BASE, EEPROM, false); // R/S = 0 (Send)

  I2CMasterDataPut(I2C3_BASE, addr_h);                          // 8 MSB
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Start
  while (I2CMasterBusy(I2C3_BASE))
    ;

  I2CMasterDataPut(I2C3_BASE, addr_l);                         // 8 LSB
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_CONT); // Continue
  while (I2CMasterBusy(I2C3_BASE))
    ;

  I2CMasterDataPut(I2C3_BASE, data);
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // Finish
  while (I2CMasterBusy(I2C3_BASE))
    ;
}

uint8_t ReadExternalEEPROM(const uint8_t addr_h, const uint8_t addr_l)
{
  I2CMasterSlaveAddrSet(I2C3_BASE, EEPROM, false); // R/S = 0 (Send)

  I2CMasterDataPut(I2C3_BASE, addr_h);                          // 8 MSB
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Start
  while (I2CMasterBusy(I2C3_BASE))
    ;

  I2CMasterDataPut(I2C3_BASE, addr_l);                           // 8 LSB
  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // Finish
  while (I2CMasterBusy(I2C3_BASE))
    ;

  I2CMasterSlaveAddrSet(I2C3_BASE, EEPROM, true); // R/S = 1 (Read)

  I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE); // Receive
  while (I2CMasterBusy(I2C3_BASE))
    ;

  uint8_t data = I2CMasterDataGet(I2C3_BASE);

  return data;
}
