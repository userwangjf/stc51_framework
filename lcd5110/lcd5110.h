

#ifndef __LCD5110_H__
#define __LCD5110_H__

//PIN
//1		RST
//2		CE/CS
//3		DC/RS
//4		DIN/DIO
//5		CLK/SCK
//6		VCC
//7		LIGHT
//8		GND

#define LCD5110_CLR_RST 	P20 = 0
#define LCD5110_SET_RST 	P20 = 1

#define LCD5110_CLR_CS  	P21 = 0
#define LCD5110_SET_CS  	P21 = 1

#define LCD5110_CLR_RS  	P22 = 0
#define LCD5110_SET_RS  	P22 = 1

#define LCD5110_CLR_DIO		P23 = 0
#define LCD5110_SET_DIO		P23 = 1

#define LCD5110_CLR_SCK		P24 = 0
#define LCD5110_SET_SCK		P24 = 1

#define LCD5110_CLR_LED		P25 = 0
#define LCD5110_SET_LED		P25 = 1

#define LCD5110_DLY			delay_us(10)

void LCD5110_Init(void);
void LCD5110_Clear(u8 dat);
void LCD5110_TxByte(u8 byte,u8 dat);
u8   LCD5110_SetXY(u8 x, u8 y);
void LCD5110_Disp5x8(u8 x, u8 y, u8* dat);
void LCD5110_Disp8x16(u8 x, u8 y, u8* dat);
void LCD5110_Disp12x24(u8 x, u8 y, u8* dat);


#endif

