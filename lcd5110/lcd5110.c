

#include "bsp/bsp.h"

#include "lcd5110/lcd5110.h"


void LCD5110_TxByte(u8 byte,u8 dat)
{
	u8 i;

	if(dat)
		LCD5110_SET_RS;
	else
		LCD5110_CLR_RS;
	LCD5110_CLR_CS;
	LCD5110_DLY;

	for(i=0;i<8;i++)
	{
		if(byte & 0x80)
			LCD5110_SET_DIO;
		else
			LCD5110_CLR_DIO;
		LCD5110_SET_SCK;
		LCD5110_DLY;
		LCD5110_CLR_SCK;
		LCD5110_DLY;
		byte = byte << 1;
	}
	LCD5110_DLY;

	LCD5110_SET_CS;
}



void LCD5110_Init(void)
{
	u16 i;

	//初始化port
	LCD5110_CLR_LED;
	LCD5110_SET_CS;
	LCD5110_CLR_RS;
	LCD5110_CLR_SCK;
	LCD5110_CLR_DIO;

    // 产生一个让LCD复位的低电平脉冲
	LCD5110_CLR_RST;
	for(i=0;i<1000;i++);
	LCD5110_SET_RST;

	//必须
	LCD5110_CLR_CS;
	for(i=0;i<100;i++);
	LCD5110_SET_CS;

    LCD5110_TxByte(0x21,0);	// 使用扩展命令设置LCD模式
	// 设置偏置电压
	//LCD5110_TxByte(0xd0,0);	
    //LCD5110_TxByte(0xc8,0);
	LCD5110_TxByte(0xc0,0);
	
    LCD5110_TxByte(0x06,0);	// 温度校正
    LCD5110_TxByte(0x13,0);	// 1:48
    LCD5110_TxByte(0x20,0);	// 使用基本命令
    LCD5110_TxByte(0x0c,0);	// 设定显示模式，正常显示




}

void LCD5110_Clear(u8 dat)
{
    u16 i;
    LCD5110_TxByte(0x0c,0);	
    LCD5110_TxByte(0x80,0);
    for (i=0; i<504; i++)
	{
        LCD5110_TxByte(dat,1);
	}
}

//上下Y,48bit，左右X,84bit
//低位在上
u8 LCD5110_SetXY(u8 x, u8 y)
{
	if(x<=83 && y<=5)
	{
    	LCD5110_TxByte(0x40 | y,0);// row行
    	LCD5110_TxByte(0x80 | x,0);// column列
		return 1;
	}
	else
		return 0;
}


//宽5,高8的英文字符，dat的长度为5
//x方向占6点，y方向占8点
void LCD5110_Disp5x8(u8 x, u8 y, u8* dat)
{
	u8 i;
	if(LCD5110_SetXY(x,y))
	{
		for(i=0;i<5;i++)
			LCD5110_TxByte(dat[i],1);	
	}
}

//宽8,高16的英文字符，dat的长度为16
void LCD5110_Disp8x16(u8 x, u8 y, u8* dat)
{
	u8 i;
	if(LCD5110_SetXY(x,y))
	{
		for(i=0;i<8;i++)
			LCD5110_TxByte(dat[i],1);
	}
	if(LCD5110_SetXY(x,y+1))
	{
		for(i=8;i<16;i++)
			LCD5110_TxByte(dat[i],1);
	}	
}

//宽12,高24的英文字符，dat的长度为36
void LCD5110_Disp12x24(u8 x, u8 y, u8* dat)
{
	u8 i;
	if(LCD5110_SetXY(x,y))
	{
		for(i=0;i<12;i++)
			LCD5110_TxByte(dat[i],1);
	}
	if(LCD5110_SetXY(x,y+1))
	{
		for(i=12;i<24;i++)
			LCD5110_TxByte(dat[i],1);
	}
	if(LCD5110_SetXY(x,y+2))
	{
		for(i=24;i<36;i++)
			LCD5110_TxByte(dat[i],1);
	}	
}
















