#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#define GPIO_PA0_U0RX 0x00000001
#define GPIO_PA1_U0TX 0x00000401

extern void ConfigUART0(void);
extern void ConfigADC1(void);
extern uint32_t Sum(const uint32_t *str);

#endif