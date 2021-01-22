

#ifndef __DS3231_H__
#define __DS3231_H__

#define DS3231_SECOND		0x00
#define DS3231_MINUTE		0x01
#define DS3231_HOUR			0x02
#define DS3231_WEEK			0x03
#define DS3231_DAY			0x04
#define DS3231_MONTH		0x05
#define DS3231_YEAR			0x06
#define DS3231_ALM1_SEC		0x07
#define DS3231_ALM1_MIN		0x08
#define DS3231_ALM1_HOUR	0x09
#define DS3231_ALM1_DAY		0x0a
#define DS3231_ALM2_MIN		0x0b
#define DS3231_ALM2_HOUR	0x0c
#define DS3231_ALM2_DAY		0x0d
#define DS3231_CTRL			0x0e
#define DS3231_STAT			0x0f
#define DS3231_AOFF			0x10
#define DS3231_TEMPH		0x11
#define DS3231_TEMPL		0x12


typedef struct
{
	u8  second;
	u8	minute;
	u8	hour;
	u8	week;
	u8	day;
	u8	month;
	u8  year;
} stSysTime;

extern stSysTime ds3231_time;

void ds3231_updateTime(void);
void ds3231_write(u8 addr, u8 byte);
u8 ds3231_read(u8 addr);
void ds3231_print_time();
u16 ds3231_getConv(void);


#endif

