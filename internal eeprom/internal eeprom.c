#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/eeprom.h"

// Custom lib
#include "functions.h"

// TM4C123GH6PM -> 2 kB EEPROM: 2048 addresses of 1 byte (8 bits) -> 512 addresses of 4 bytes (32 bits)

volatile uint32_t Sum = 0, Biggest = 0, Lowest = 0;

void ReadEEPROM(void) // Reads specific addresses
{
  char status = 0;

  mem_t read =
      {
          .data = {0},
          .initial_addr = 0,
          .final_addr = 0};

  UARTprintf("\x1B[2J\x1B[0;0H");

  UARTprintf("Initial address: ");
  read.initial_addr = UARTDecGet(UART0_BASE);
  while (read.initial_addr >= (sizeof(read.data) / 4))
  {
    UARTprintf("\n\nInitial address must be between 0 and 511, try again: ");
    read.initial_addr = UARTDecGet(UART0_BASE);
  }

  UARTprintf("\n\nFinal address: ");
  read.final_addr = UARTDecGet(UART0_BASE);
  while (read.final_addr >= (sizeof(read.data) / 4) || read.final_addr < read.initial_addr)
  {
    UARTprintf("\n\nFinal address must be between 0 and 511, and bigger or equal to the initial address, try again: ");
    read.final_addr = UARTDecGet(UART0_BASE);
  }

  UARTprintf("\n");
  EEPROMRead(read.data, 0, sizeof(read.data));
  for (register uint16_t i = read.initial_addr; i <= read.final_addr; i++)
  {
    // Wide console to read it cleaner
    if (i % 10 == 0)
      UARTprintf("\n %3d -> ", i);
    UARTprintf("%12d ", read.data[i]);
  }

  UARTprintf("\n\nx -> Go back");
  while (status != 'x')
    status = UARTCharGet(UART0_BASE);
}

void WriteEEPROM(void) // Writes given values on specified addresses
{
  uint16_t Quant = 0;
  char status = 0;

  mem_t write =
      {
          .data = {0},
          .initial_addr = 0,
          .final_addr = 0};

  UARTprintf("\x1B[2J\x1B[0;0H");

  UARTprintf("Enter how many numbers are going to be written: ");
  Quant = UARTDecGet(UART0_BASE);
  while ((Quant >= (sizeof(write.data) / 4)) || (Quant <= 0))
  {
    UARTprintf("\n\nIt must be between 1 and 511, try again: ");
    Quant = UARTDecGet(UART0_BASE);
  }

  UARTprintf("\n\nEnter the initial address: ");
  write.initial_addr = UARTDecGet(UART0_BASE);
  while (write.initial_addr >= (sizeof(write.data) / 4) || write.initial_addr > ((sizeof(write.data) / 4) - Quant))
  {
    UARTprintf("\n\nInitial address must be between 0 and %i, try again: ", (sizeof(write.data) / 4) - Quant);
    write.initial_addr = UARTDecGet(UART0_BASE);
  }

  UARTprintf("\n\nEnter the numbers that are going to be written: \n\n");
  for (register uint16_t i = 0; i < Quant; i++)
  {
    UARTprintf(" [%d] - ", i + 1);
    write.data[i] = UARTDecGet(UART0_BASE);
    while (write.data[i] > 2147483647) // (2 ^ 31) - 1
    {
      UARTprintf("\n Maximum value is (2^31)-1, try again: ");
      UARTprintf("\n [%d] - ", i + 1);
      write.data[i] = UARTDecGet(UART0_BASE);
    }
    UARTprintf("\n");
  }
  EEPROMProgram(write.data, write.initial_addr * 4, Quant * 4);

  write.final_addr = write.initial_addr + Quant - 1;

  UARTprintf("\n%i numbers written on EEPROM from %i to %i", Quant, write.initial_addr, write.final_addr);

  UARTprintf("\n\nx -> Go back");
  while (status != 'x')
    status = UARTCharGet(UART0_BASE);
}

void ValuesEEPROM(void) // Finds the sum, biggest and lowest values
{
  uint32_t Data[512] = {0}, Prev = 0, PrevB = 2147483647; // (2 ^ 31) - 1
  char status = 0;

  UARTprintf("\x1B[2J\x1B[0;0H");

  EEPROMRead(Data, 0, sizeof(Data));
  Sum = 0;
  Biggest = 0;
  Lowest = 0;
  for (register uint16_t i = 0; i < (sizeof(Data) / 4); i++)
  {
    if (Data[i] != -1)
    {
      Sum += Data[i];

      if (Data[i] >= Prev)
      {
        Biggest = Data[i];
        Prev = Biggest;
      }

      if (Data[i] <= PrevB)
      {
        Lowest = Data[i];
        PrevB = Lowest;
      }
    }
  }

  if (Sum <= 2147483647)
    UARTprintf("Sum = %i", Sum);
  else
    UARTprintf("Sum > (2^31)-1");

  UARTprintf("\n\nBiggest = %i", Biggest);
  UARTprintf("\n\nLowest = %i", Lowest);

  UARTprintf("\n\nx -> Go back");
  while (status != 'x')
    status = UARTCharGet(UART0_BASE);
}

void DeleteEEPROM(void) // Erase specific addresses
{
  uint16_t Quant = 0;

  mem_t del =
      {
          .data = {0},
          .initial_addr = 0,
          .final_addr = 0};

  UARTprintf("\x1B[2J\x1B[0;0H");

  UARTprintf("Enter the initial address: ");
  del.initial_addr = UARTDecGet(UART0_BASE);
  while (del.initial_addr >= (sizeof(del.data) / 4))
  {
    UARTprintf("\n\nThe initial address must be between 0 and 511, try again: ");
    del.initial_addr = UARTDecGet(UART0_BASE);
  }

  UARTprintf("\n\nEnter the final address: ");
  del.final_addr = UARTDecGet(UART0_BASE);
  while (del.final_addr >= (sizeof(del.data) / 4) || del.final_addr < del.initial_addr)
  {
    UARTprintf("\n\nThe final address must be between 0 and 511, bigger or equal to the initial address, try again: ");
    del.final_addr = UARTDecGet(UART0_BASE);
  }

  Quant = del.final_addr - del.initial_addr + 1;
  for (register uint32_t i = 0; i < Quant; i++)
  {
    del.data[i] = 0xFFFFFFFF;
  }
  EEPROMProgram(del.data, del.initial_addr * 4, Quant * 4);

  UARTprintf("\n\nEEPROM erased from %i to %i", del.initial_addr, del.final_addr);
}

void FullyEraseEEPROM(void)
{
  char status = 0;

  UARTprintf("\x1B[2J\x1B[0;0H");

  UARTprintf("Press Any Key to Confirm");
  UARTprintf("\n\nx -> Go back");

  status = UARTCharGet(UART0_BASE);
  if (status != 'x')
  {
    UARTprintf("\x1B[2J\x1B[0;0H");
    UARTprintf("Erasing EEPROM");

    EEPROMMassErase();
    for (uint8_t k = 0; k <= 2; k++)
    {
      UARTprintf(".");
      for (uint32_t i = 0; i < 1000000; i++)
        ; // Delay
    }

    UARTprintf("\n\nCompleted");
    UARTprintf("\n\nx -> Exit");
    while (status != 'x')
      status = UARTCharGet(UART0_BASE);
  }
}

void Start(void)
{
  char status = 0;

  do
  {
    UARTprintf("\x1B[2J\x1B[0;0H"); // Clear console

    UARTprintf("1 -> Read EEPROM\n2 -> Write EEPROM\n3 -> Sum, biggest and lowest value on EEPROM");
    UARTprintf("\n4 -> Erase specific addresses\n5 -> Fully erase EEPROM\n\nx -> Exit program");

    status = UARTCharGet(UART0_BASE);

    switch (status)
    {
    case '1': // Read
      ReadEEPROM();
      break;

    case '2': // Write
      WriteEEPROM();
      break;

    case '3': // Sum, biggest and lowest value
      ValuesEEPROM();
      break;

    case '4': // Erase specific addresses
      DeleteEEPROM();
      break;

    case '5': // Fully erase
      FullyEraseEEPROM();
      break;

    case 'x': // Exit program
      break;

    default:
      UARTprintf("\nInvalid option");
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

  UARTprintf("\x1B[2J\x1B[0;0H"); // Clear console

  ConfigEEPROM0();

  Start();
}