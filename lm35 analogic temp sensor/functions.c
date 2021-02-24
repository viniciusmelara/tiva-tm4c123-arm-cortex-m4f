#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
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

void ConfigADC1(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
  ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
  for (uint8_t i = 0; i <= 6; i++)
    ADCSequenceStepConfigure(ADC1_BASE, 0, i, ADC_CTL_CH7);
  ADCSequenceStepConfigure(ADC1_BASE, 0, 7, ADC_CTL_CH7 | ADC_CTL_IE | ADC_CTL_END);
  ADCHardwareOversampleConfigure(ADC1_BASE, 64);
  ADCSequenceEnable(ADC1_BASE, 0);
}

uint32_t Sum(const uint32_t *str)
{
  uint32_t value = 0;

  for (register uint8_t i = 0; i <= (sizeof(str) - 1); i++)
    value += *(str + i);

  return value;
}