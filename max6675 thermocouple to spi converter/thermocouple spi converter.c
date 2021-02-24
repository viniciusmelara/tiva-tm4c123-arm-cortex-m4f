#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/hw_ssi.h"
#include "driverlib/ssi.h"

// Custom lib
#include "functions.h"

uint32_t received = 0;

void SSI0Read(void)
{
  SSIDataPut(SSI0_BASE, 0x00); // Clock
  SSIDataGet(SSI0_BASE, &received);
  while (SSIBusy(SSI0_BASE))
    ;
}

void main()
{
  ConfigUART0();
  ConfigSSI0();

  UARTprintf("\x1B[2J\x1B[0;0H");
  UARTprintf("MAX6675 - Thermocouple to SPI Converter");
  UARTprintf("\n\nThermocouple: type K, 0 to 1023,5 C\n\n");

  while (true)
  {
    uint32_t sum = 0, mean = 0;
    float temp = 0;
    char s_temp[6] = {0};

    for (register uint8_t j = 0; j < 10; j++)
    {
      SSI0Read();
      sum += received;
    }

    mean = sum / 10;
    temp = (mean >> 3) * 0.25;

    sprintf(s_temp, "%3.1f", temp);
    UARTprintf("Temperature = %s C\r", s_temp);
    SysCtlDelay(3000000);
  }
}