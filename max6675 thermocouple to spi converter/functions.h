#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#define GPIO_PA0_U0RX 0x00000001
#define GPIO_PA1_U0TX 0x00000401

#define GPIO_PA2_SSI0CLK 0x00000802
#define GPIO_PA3_SSI0FSS 0x00000C02
#define GPIO_PA4_SSI0RX 0x00001002

extern void ConfigUART0(void);
extern void ConfigSSI0(void);

#endif