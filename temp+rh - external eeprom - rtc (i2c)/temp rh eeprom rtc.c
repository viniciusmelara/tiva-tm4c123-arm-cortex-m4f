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

// Custom lib
#include "functions.h"

void MonitorRealTime(void)
{
  char status = 0, sec_old = 60;
  time_t current;
  save_t value = {
      .biggestRH = 0,
      .lowestRH = 101,
      .biggestTemp = -50,
      .lowestTemp = 150};

  current.sec = GetRTC(SEC);
  current.min = GetRTC(MIN);
  current.hour = GetRTC(HRS);
  current.day = GetRTC(DAY);
  current.month = GetRTC(MONTH);
  current.year = GetRTC(YEAR);

  do
  {
    current.sec = GetRTC(SEC);

    if (current.sec != sec_old)
    {
      UARTprintf("\x1B[2J\x1B[0;0H");

      if (current.sec == 0)
      {
        current.min = GetRTC(MIN);
        if (current.min == 0)
        {
          current.hour = GetRTC(HRS);
          if (current.hour == 0)
          {
            current.day = GetRTC(DAY);
            if (current.day == 1)
            {
              current.month = GetRTC(MONTH);
              if (current.month == 1)
                current.year = GetRTC(YEAR);
            }
          }
        }
      }

      uint16_t current_rh = 0;
      current_rh = (GetHTU(TRIG_RH) * 125 / 65535) - 6; // RH%

      int32_t current_temp = 0;
      current_temp = (((4393 / 25) * GetHTU(TRIG_TEMP)) / 65535) - (937 / 20); // �C

      UARTprintf("Current Relative Humidity - %3i RH\r", current_rh);
      UARTprintf("\nCurrent Temperature - %3i C\r", current_temp);

      UARTprintf("\n\nCurrent time - %02d:%02d:%02d\r", current.hour, current.min, current.sec);
      UARTprintf("\nCurrent date - %02d/%02d/%02d\r", current.day, current.month, current.year);

      UARTprintf("\n\nx -> Go back");

      if (current_temp < 20)
        LEDS = SKY;
      else if (current_temp >= 20 && current_temp < 40)
        LEDS = GREEN;
      else if (current_temp >= 40 && current_temp < 60)
        LEDS = YELLOW;
      else if (current_temp >= 60)
        LEDS = RED;

      if (current_rh > value.biggestRH)
      {
        value.biggestRH = current_rh;

        WriteExternalEEPROM(0x00, RH_MAX, value.biggestRH);

        WriteExternalEEPROM(0x00, RH_SEC_MAX, current.sec);
        WriteExternalEEPROM(0x00, RH_MIN_MAX, current.min);
        WriteExternalEEPROM(0x00, RH_HRS_MAX, current.hour);

        WriteExternalEEPROM(0x00, RH_DAY_MAX, current.day);
        WriteExternalEEPROM(0x00, RH_MONTH_MAX, current.month);
        WriteExternalEEPROM(0x00, RH_YEAR_MAX, current.year);
      }

      if (current_rh < value.lowestRH)
      {
        value.lowestRH = current_rh;
        WriteExternalEEPROM(0x00, RH_MIN, value.lowestRH);

        WriteExternalEEPROM(0x00, RH_SEC_MIN, current.sec);
        WriteExternalEEPROM(0x00, RH_MIN_MIN, current.min);
        WriteExternalEEPROM(0x00, RH_HRS_MIN, current.hour);

        WriteExternalEEPROM(0x00, RH_DAY_MIN, current.day);
        WriteExternalEEPROM(0x00, RH_MONTH_MIN, current.month);
        WriteExternalEEPROM(0x00, RH_YEAR_MIN, current.year);
      }

      if (current_temp > value.biggestTemp)
      {
        value.biggestTemp = current_temp;
        WriteExternalEEPROM(0x00, TEMP_MAX, value.biggestTemp);

        WriteExternalEEPROM(0x00, TEMP_SEC_MAX, current.sec);
        WriteExternalEEPROM(0x00, TEMP_MIN_MAX, current.min);
        WriteExternalEEPROM(0x00, TEMP_HRS_MAX, current.hour);

        WriteExternalEEPROM(0x00, TEMP_DAY_MAX, current.day);
        WriteExternalEEPROM(0x00, TEMP_MONTH_MAX, current.month);
        WriteExternalEEPROM(0x00, TEMP_YEAR_MAX, current.year);
      }

      if (current_temp < value.lowestTemp)
      {
        value.lowestTemp = current_temp;
        WriteExternalEEPROM(0x00, TEMP_MIN, value.lowestTemp);

        WriteExternalEEPROM(0x00, TEMP_SEC_MIN, current.sec);
        WriteExternalEEPROM(0x00, TEMP_MIN_MIN, current.min);
        WriteExternalEEPROM(0x00, TEMP_HRS_MIN, current.hour);

        WriteExternalEEPROM(0x00, TEMP_DAY_MIN, current.day);
        WriteExternalEEPROM(0x00, TEMP_MONTH_MIN, current.month);
        WriteExternalEEPROM(0x00, TEMP_YEAR_MIN, current.year);
      }

      sec_old = current.sec;
    }

    if (UARTCharsAvail(UART0_BASE))
      status = UARTCharGet(UART0_BASE);
  } while (status != 'x');
}

void ValuesExternalEEPROM(void)
{
  char status = 0;
  time_t dateMaxRH;
  time_t dateMinRH;
  time_t dateMaxTemp;
  time_t dateMinTemp;
  save_t value;

  value.biggestRH = ReadExternalEEPROM(0x00, RH_MAX);
  dateMaxRH.sec = ReadExternalEEPROM(0x00, RH_SEC_MAX);
  dateMaxRH.min = ReadExternalEEPROM(0x00, RH_MIN_MAX);
  dateMaxRH.hour = ReadExternalEEPROM(0x00, RH_HRS_MAX);
  dateMaxRH.day = ReadExternalEEPROM(0x00, RH_DAY_MAX);
  dateMaxRH.month = ReadExternalEEPROM(0x00, RH_MONTH_MAX);
  dateMaxRH.year = ReadExternalEEPROM(0x00, RH_YEAR_MAX);

  value.lowestRH = ReadExternalEEPROM(0x00, RH_MIN);
  dateMinRH.sec = ReadExternalEEPROM(0x00, RH_SEC_MIN);
  dateMinRH.min = ReadExternalEEPROM(0x00, RH_MIN_MIN);
  dateMinRH.hour = ReadExternalEEPROM(0x00, RH_HRS_MIN);
  dateMinRH.day = ReadExternalEEPROM(0x00, RH_DAY_MIN);
  dateMinRH.month = ReadExternalEEPROM(0x00, RH_MONTH_MIN);
  dateMinRH.year = ReadExternalEEPROM(0x00, RH_YEAR_MIN);

  value.biggestTemp = ReadExternalEEPROM(0x00, TEMP_MAX);
  dateMaxTemp.sec = ReadExternalEEPROM(0x00, TEMP_SEC_MAX);
  dateMaxTemp.min = ReadExternalEEPROM(0x00, TEMP_MIN_MAX);
  dateMaxTemp.hour = ReadExternalEEPROM(0x00, TEMP_HRS_MAX);
  dateMaxTemp.day = ReadExternalEEPROM(0x00, TEMP_DAY_MAX);
  dateMaxTemp.month = ReadExternalEEPROM(0x00, TEMP_MONTH_MAX);
  dateMaxTemp.year = ReadExternalEEPROM(0x00, TEMP_YEAR_MAX);

  value.lowestTemp = ReadExternalEEPROM(0x00, TEMP_MIN);
  dateMinTemp.sec = ReadExternalEEPROM(0x00, TEMP_SEC_MIN);
  dateMinTemp.min = ReadExternalEEPROM(0x00, TEMP_MIN_MIN);
  dateMinTemp.hour = ReadExternalEEPROM(0x00, TEMP_HRS_MIN);
  dateMinTemp.day = ReadExternalEEPROM(0x00, TEMP_DAY_MIN);
  dateMinTemp.month = ReadExternalEEPROM(0x00, TEMP_MONTH_MIN);
  dateMinTemp.year = ReadExternalEEPROM(0x00, TEMP_YEAR_MIN);

  UARTprintf("\x1B[2J\x1B[0;0H"); // Clear console

  UARTprintf("Maximum Relative Humidity Registered - %3i RH at %02d:%02d:%02d of %02d/%02d/%02d\r", value.biggestRH,
             dateMaxRH.hour, dateMaxRH.min, dateMaxRH.sec, dateMaxRH.day, dateMaxRH.month, dateMaxRH.year);

  UARTprintf("\nMinimum Relative Humidity Registered - %3i RH at %02d:%02d:%02d of %02d/%02d/%02d\r", value.lowestRH,
             dateMinRH.hour, dateMinRH.min, dateMinRH.sec, dateMinRH.day, dateMinRH.month, dateMinRH.year);

  UARTprintf("\n\nMaximum Temperature Registered - %3i C at %02d:%02d:%02d of %02d/%02d/%02d\r", value.biggestTemp,
             dateMaxTemp.hour, dateMaxTemp.min, dateMaxTemp.sec, dateMaxTemp.day, dateMaxTemp.month, dateMaxTemp.year);

  UARTprintf("\nMinimum Temperature Registered - %3i C at %02d:%02d:%02d of %02d/%02d/%02d\r", value.lowestTemp,
             dateMinTemp.hour, dateMinTemp.min, dateMinTemp.sec, dateMinTemp.day, dateMinTemp.month, dateMinTemp.year);

  UARTprintf("\n\nx -> Go back");
  while (status != 'x')
    status = UARTCharGet(UART0_BASE);
}

void EraseValuesExternalEEPROM()
{
  char status = 0;

  UARTprintf("\x1B[2J\x1B[0;0H"); // Clear console

  UARTprintf("Press Any Key to Confirm");
  UARTprintf("\n\nx -> Go back");

  status = UARTCharGet(UART0_BASE);
  if (status != 'x')
  {
    UARTprintf("\x1B[2J\x1B[0;0H");
    UARTprintf("Erasing Stored Values: ");

    for (register uint8_t mem_addr = RH_MIN; mem_addr <= TEMP_YEAR_MAX; mem_addr++)
    {
      WriteExternalEEPROM(0x00, mem_addr, 0xFF);
      for (uint32_t i = 0; i < 100000; i++)
        ; // Delay
      UARTprintf(".");
    }

    UARTprintf("\n\nCompleted");
    UARTprintf("\n\nx -> Exit");
    while (status != 'x')
      status = UARTCharGet(UART0_BASE);
  }
}

void SetDateAndTime()
{
  char status = 0, max_days = 0;

  time_t set;

  do
  {
    UARTprintf("\x1B[2J\x1B[0;0H"); // Clear console

    UARTprintf("1 -> Set Time");
    UARTprintf("\n2 -> Set Date");
    UARTprintf("\n\nx -> Exit");

    status = UARTCharGet(UART0_BASE);

    switch (status)
    {
    case '1':
      UARTprintf("\x1B[2J\x1B[0;0H");
      UARTprintf("Enter Hour: ");

      set.hour = UARTDecGet(UART0_BASE);
      while (set.hour > 23)
      {
        UARTprintf("\n\nInvalid");
        for (uint32_t i = 0; i < 1000000; i++)
          ; // Delay
        UARTprintf("\r                ");

        UARTprintf("\x1B[2J\x1B[0;0H");
        UARTprintf("Enter Hour: ");
        set.hour = UARTDecGet(UART0_BASE);
      }

      UARTprintf("\nEnter Minute: ");

      set.min = UARTDecGet(UART0_BASE);
      while (set.min > 59)
      {
        UARTprintf("\n\nInvalid");
        for (uint32_t i = 0; i < 1000000; i++)
          ;
        UARTprintf("\r                ");

        UARTprintf("\x1B[2J\x1B[0;0H");
        UARTprintf("Enter Hour: %02d", set.hour);
        UARTprintf("\nEnter Minute: ");
        set.min = UARTDecGet(UART0_BASE);
      }

      SetRTC(TIME, 0x00, set.min, set.hour); // Set Time
      UARTprintf("\n\nTime Set to: [%02d:%02d:00]", set.hour, set.min);

      UARTprintf("\n\nx -> Exit");
      while (status != 'x')
        status = UARTCharGet(UART0_BASE);
      break;

    case '2':
      UARTprintf("\x1B[2J\x1B[0;0H");
      UARTprintf("Enter Year: ");

      set.year = UARTDecGet(UART0_BASE);
      while (set.year > 9999)
      {
        UARTprintf("\n\nInvalid");
        for (uint32_t i = 0; i < 1000000; i++)
          ;
        UARTprintf("\r                ");

        UARTprintf("\x1B[2J\x1B[0;0H");
        UARTprintf("Enter Year: ");
        set.year = UARTDecGet(UART0_BASE);
      }

      UARTprintf("\nEnter Month: ");

      set.month = UARTDecGet(UART0_BASE);
      while (set.month > 12 || set.month == 0)
      {
        UARTprintf("\n\nInvalid");
        for (uint32_t i = 0; i < 1000000; i++)
          ;
        UARTprintf("\r                ");

        UARTprintf("\x1B[2J\x1B[0;0H");
        UARTprintf("Enter Year: %02d", set.year);
        UARTprintf("\nEnter Month: ");
        set.month = UARTDecGet(UART0_BASE);
      }

      if (set.month == 1 || set.month == 3 || set.month == 5 || set.month == 7 || set.month == 8 || set.month == 10 || set.month == 12)
        max_days = 31;
      else if (set.month == 4 || set.month == 6 || set.month == 9 || set.month == 11)
        max_days = 30;
      else if (set.month == 2)
      {
        if ((set.year % 4 == 0 && set.year % 100 != 0) || set.year % 400 == 0)
          max_days = 29;
        else
          max_days = 28;
      }

      UARTprintf("\nEnter Day: ");

      set.day = UARTDecGet(UART0_BASE);
      while (set.day > max_days || set.day == 0)
      {
        UARTprintf("\n\nInvalid");
        for (uint32_t i = 0; i < 1000000; i++)
          ;
        UARTprintf("\r                ");

        UARTprintf("\x1B[2J\x1B[0;0H");
        UARTprintf("Enter Year: %02d", set.year);
        UARTprintf("\nEnter Month: %02d", set.month);
        UARTprintf("\nEnter Day: ");
        set.day = UARTDecGet(UART0_BASE);
      }

      SetRTC(DATE, set.day, set.month, set.year); // Set Date
      UARTprintf("\n\nDate Set to: [%02d/%02d/%02d]", set.day, set.month, set.year);

      UARTprintf("\n\nx -> Exit");
      while (status != 'x')
        status = UARTCharGet(UART0_BASE);
      break;

    case 'x':
      break;

    default:
      UARTprintf("\n\nInvalid option");
      for (uint32_t i = 0; i < 1000000; i++)
        ;
      UARTprintf("\r                ");
      break;
    }
  } while (status != 'x');
}

void Start(void)
{
  char status = 0;

  do
  {
    UARTprintf("\x1B[2J\x1B[0;0H"); // Clear console

    UARTprintf("1 -> Monitor Time, Temperature and Relative Humidity");
    UARTprintf("\n2 -> See Max and Min Values Stored in External EEPROM");
    UARTprintf("\n3 -> Reset Stored Values");
    UARTprintf("\n4 -> Set Current Date/Time");
    UARTprintf("\n\nx -> Exit");

    LEDS = 0;

    status = UARTCharGet(UART0_BASE);

    switch (status)
    {
    case '1':
      MonitorRealTime();
      break;

    case '2':
      ValuesExternalEEPROM();
      break;

    case '3':
      EraseValuesExternalEEPROM();
      break;

    case '4':
      SetDateAndTime();
      break;

    case 'x': // Exit program
      break;

    default:
      UARTprintf("\n\nInvalid option");
      for (uint32_t i = 0; i < 1000000; i++)
        ; // Delay
      UARTprintf("\r                ");
      break;
    }
  } while (status != 'x');

  UARTprintf("\x1B[2J\x1B[0;0H");
  UARTprintf("THE END");
}

void main(void)
{
  ConfigUART0();
  ConfigI2C3();
  ConfigLedRGB();

  Start();
}