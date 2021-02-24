#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#define GPIO_PA0_U0RX 0x00000001
#define GPIO_PA1_U0TX 0x00000401

typedef struct mem
{
  uint32_t data[512];
  uint16_t initial_addr;
  uint16_t final_addr;
} mem_t;

extern void ConfigUART0(void);
extern void ConfigEEPROM0(void);
extern uint32_t UARTDecGet(const uint32_t uartx_base);

#endif