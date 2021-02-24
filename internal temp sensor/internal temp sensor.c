#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"

// Custom lib
#include "functions.h"

void main(void)
{
  uint32_t ADC0Value[4] = {0}, TempAvg = 0, TempC = 0;

  ConfigUART0();
  ConfigADC0();

  while (true)
  {
    ADCIntClear(ADC0_BASE, 1);
    ADCProcessorTrigger(ADC0_BASE, 1);
    while (!ADCIntStatus(ADC0_BASE, 1, false))
      ;
    ADCSequenceDataGet(ADC0_BASE, 1, ADC0Value);

    TempAvg = Sum(ADC0Value) / 4;
    TempC = (1475 - (2475 * TempAvg) / 4096) / 10;

    UARTprintf("\x1B[2J\x1B[0;0H"); // Clear console
    UARTprintf("INTERNAL TEMPERATURE SENSOR\n");

    for (uint8_t i = 0; i <= 3; i++)
      UARTprintf("\n Read %d: = %d", i + 1, ADC0Value[i]);

    UARTprintf("\n Mean = %4d\n", TempAvg);
    UARTprintf("\nTEMPERATURE = %02d C", TempC);

    SysCtlDelay(2222222);
  }
}
