#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

// Custom lib
#include "functions.h"

void main(void)
{
  uint32_t ADC1Value[8] = {0}, Temp, Temp_Dec, Temp_Dec2;

  ConfigUART0();
  ConfigADC1();

  while (true)
  {
    UARTprintf("\x1B[2J\x1B[0;0H"); // Clear console

    ADCIntClear(ADC1_BASE, 0);
    ADCProcessorTrigger(ADC1_BASE, 0);
    while (!ADCIntStatus(ADC1_BASE, 0, false))
      ;
    ADCSequenceDataGet(ADC1_BASE, 0, ADC1Value);

    Temp = (Sum(ADC1Value) / 8) * 33 / 40950;
    Temp_Dec = ((Sum(ADC1Value) / 8) * 3300 / 4095) - Temp * 10;
    Temp_Dec2 = Temp_Dec % 10;

    UARTprintf("LM35 TEMPERATURE SENSOR\n\n");

    UARTprintf("Temperature = %02d,%d C\r", Temp_Dec / 10, Temp_Dec2);

    SysCtlDelay(2222222);
  }
}
