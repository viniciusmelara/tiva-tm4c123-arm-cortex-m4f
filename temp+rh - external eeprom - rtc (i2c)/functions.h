#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

// I2C3
#define GPIO_PD0_I2C3SCL 0x00030003
#define GPIO_PD1_I2C3SDA 0x00030403

// UART0
#define GPIO_PA0_U0RX 0x00000001
#define GPIO_PA1_U0TX 0x00000401

// SetRTC() function start_addr
#define TIME 0x00
#define DATE 0x01

// DS1307 (RTC) Addresses
#define RTC 0x68
#define SEC 0x00
#define MIN 0x01
#define HRS 0x02
#define DAYW 0x03
#define DAY 0x04
#define MONTH 0x05
#define YEAR 0x06
#define CNTRL 0x07

// HTU21D Addresses
#define HTU 0x40
#define TRIG_TEMP 0xE3
#define TRIG_RH 0xE5
#define WRITE 0xE6
#define READ 0xE7

// EEPROM addresses to store max. and min. values
#define RH_MAX 0x08
#define RH_SEC_MAX 0x09
#define RH_MIN_MAX 0x0A
#define RH_HRS_MAX 0x0B
#define RH_DAY_MAX 0x0C
#define RH_MONTH_MAX 0x0D
#define RH_YEAR_MAX 0x0E

#define RH_MIN 0x00
#define RH_SEC_MIN 0x01
#define RH_MIN_MIN 0x02
#define RH_HRS_MIN 0x03
#define RH_DAY_MIN 0x04
#define RH_MONTH_MIN 0x05
#define RH_YEAR_MIN 0x06
/*-------------------------*/
#define TEMP_MAX 0x18
#define TEMP_SEC_MAX 0x19
#define TEMP_MIN_MAX 0x1A
#define TEMP_HRS_MAX 0x1B
#define TEMP_DAY_MAX 0x1C
#define TEMP_MONTH_MAX 0x1D
#define TEMP_YEAR_MAX 0x1E

#define TEMP_MIN 0x10
#define TEMP_SEC_MIN 0x11
#define TEMP_MIN_MIN 0x12
#define TEMP_HRS_MIN 0x13
#define TEMP_DAY_MIN 0x14
#define TEMP_MONTH_MIN 0x15
#define TEMP_YEAR_MIN 0x16

// 24LC512 (external EEPROM) Address
#define EEPROM 0x50 //0b1010_000 <-> 0bControlCode_A2A1A0

// LEDS
#define LEDS (*((volatile long *)0x40025038))
#define RED 0x02
#define BLUE 0x04
#define GREEN 0x08
#define YELLOW 0x0A
#define SKY 0x0C
#define WHITE 0x0E
#define PINK 0x06

typedef struct time
{
  uint8_t hour;
  uint8_t min;
  uint8_t sec;

  uint8_t day;
  uint8_t month;
  uint16_t year;
} time_t;

typedef struct save
{
  uint16_t biggestRH;
  uint16_t lowestRH;
  int32_t biggestTemp;
  int32_t lowestTemp;
} save_t;

extern void ConfigUART0(void);
extern void ConfigI2C3(void);
extern void ConfigLedRGB(void);
extern uint32_t UARTDecGet(const uint32_t uartx_base);
extern uint8_t Dec2BCD(uint8_t val);
extern uint8_t BCD2Dec(uint8_t val);
extern void SetRTC(const uint8_t mode, const uint8_t sec_day, const uint8_t min_month, const uint8_t hour_year);
extern uint8_t GetRTC(const uint8_t addr);
extern uint16_t GetHTU(const uint8_t addr);
extern void WriteExternalEEPROM(const uint8_t addr_h, const uint8_t addr_l, const uint8_t data);
extern uint8_t ReadExternalEEPROM(const uint8_t addr_h, const uint8_t addr_l);

#endif