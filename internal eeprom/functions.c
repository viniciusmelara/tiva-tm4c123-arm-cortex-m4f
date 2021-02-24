#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/eeprom.h"

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

void ConfigEEPROM0(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
  //EEPROMMassErase(); // Start with EEPROM fully erased

  if (EEPROMInit() == EEPROM_INIT_OK)
  {
    UARTprintf("EEPROM Status: ");
    for (register uint32_t i = 0; i < 1000000; i++)
      ; // Delay
    UARTprintf("OK");
  }
  else if (EEPROMInit() == EEPROM_INIT_ERROR)
  {
    UARTprintf("EEPROM Status: ");
    for (register uint32_t i = 0; i < 1000000; i++)
      ; // Delay
    UARTprintf("ERROR");
  }

  for (register uint32_t i = 0; i < 1000000; i++)
    ; // Delay
}

uint32_t UARTDecGet(const uint32_t uartx_base)
{
  uint32_t num = 0, size = 0;
  uint8_t chr = 0;

  chr = UARTCharGet(uartx_base);
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