
#ifndef __LED8_595_H__
#define __LED8_595_H__

#include "bsp/config.h"

#define IO_595_RSTn			P20
#define IO_595_SCK			P21
#define IO_595_LOAD 		P22
#define IO_595_OEn			P23
#define IO_595_SI_0			P24
#define IO_595_SI_1			P25
#define IO_595_SI_2			P26
#define IO_595_SI_3			P27

#define IO_595_POWER_OFF	P37

void led8_595_num(u8 d0,u8 d1,u8 d2,u8 d3);
void led8_595_dout(u8 d0, u8 d1, u8 d2, u8 d3);
void led8_595_scan();
void led8_bright_set(u8 bright);

#endif

