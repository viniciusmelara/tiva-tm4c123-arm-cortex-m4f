#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"

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

void ConfigADC0(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  ADCHardwareOversampleConfigure(ADC0_BASE, 64);
  ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
  for (uint8_t i = 0; i <= 2; i++)
    ADCSequenceStepConfigure(ADC0_BASE, 1, i, ADC_CTL_TS);
  ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
  ADCSequenceEnable(ADC0_BASE, 1);
}

uint32_t Sum(const uint32_t *str)
{
  uint32_t value = 0;

  for (register uint8_t i = 0; i <= (sizeof(str) - 1); i++)
    value += *(str + i);

  return value;
}