

#include "bsp/config.h"

u16 xdata disp_buf[21] = {0xff,0xff,0};

//4*6点阵
//先左再右
//先上再下
//先高4bit，然后低4bit。
//0

u8 led_dot_hex[16*3] = 
{
0x75,0x55,0x70,//0
0x22,0x22,0x20,//1
0x74,0x71,0x70,//2
0x74,0x74,0x70,//3
0x55,0x74,0x40,//4
0x71,0x74,0x70,//5
0x71,0x75,0x70,//6
0x74,0x44,0x40,//7
0x75,0x75,0x70,//8
0x75,0x74,0x70,//9
0x25,0x75,0x50,//A
0x11,0x75,0x70,//b
0x71,0x11,0x70,//C
0x44,0x75,0x70,//d
0x71,0x71,0x70,//E
0x71,0x71,0x10,//F
};


void disp_dec(u8 x, u8 y, u8 dec)
{
	u8 i;
	u16 row;
	if(y > LED_WY-5)return; 
	if(x > LED_WX-3)return;

	for(i=0;i<3;i++)
	{
		row = led_dot_hex[dec*3+i];
		row = ((row & 0x00f0) >> 4) << x;
		disp_buf[y+i*2+0] |= row;
		row = led_dot_hex[dec*3+i];
		row = ((row ^ 0x00f0)>>4)<<x;
		disp_buf[y+i*2+0] &= ~row;

		row = led_dot_hex[dec*3+i];
		row = (row & 0x000f) << x;
		disp_buf[y+i*2+1] |= row;
		row = led_dot_hex[dec*3+i];
		row = (row ^ 0x000f) << x;
		disp_buf[y+i*2+1] &= ~row;
		
	}			
}

void disp_on(u8 x, u8 y)
{
	if(x < LED_WX && y < LED_WY)
		disp_buf[y] |= (0x01 << x);
}

void disp_off(u8 x, u8 y)
{
	if(x < LED_WX && y < LED_WY)
		disp_buf[y] &= ((0x01 << x) ^ 0x07ff);	
}

//清除屏幕
void disp_clear()
{
	u8 y;
	for(y=0;y<LED_WY;y++)
	{
		disp_buf[y] = 0;
	}
}

//按行画
void disp_row(u8 y,u16 row)
{
	if(y<LED_WY)
		disp_buf[y] = row;
}

//按行清除屏幕
void disp_clear_row(u8 start,u8 end)
{
	u8 y;
	if(end >= LED_WY)end = LED_WY - 1;
	for(y=start;y<=end;y++)
	{
		disp_buf[y] = 0;
	}
}

//先写满屏幕，然后清除
void disp_clear2(u8 delay)
{
	u8 y;
	for(y=0;y<LED_WY;y++)
	{
		disp_buf[y] = 0x7ff;
		delay_ms(delay);
	}
	delay_ms(delay);
	for(y=0;y<LED_WY;y++)
	{
		disp_buf[y] = 0;
		delay_ms(delay);
	}
}

//推拉门开门的清屏动作
void disp_open(u8 delay)
{
	u8 y;
	for(y=0;y<LED_WY/2;y++)
	{
		disp_buf[y] = 0x7ff;
		disp_buf[LED_WY-y-1] = 0x7ff;
		delay_ms(delay);
	}
	disp_buf[LED_WY/2] = 0x7ff;
	delay_ms(delay);
	delay_ms(delay);
	disp_buf[LED_WY/2] = 0;
	for(y=0;y<LED_WY/2;y++)
	{
		delay_ms(delay);
		disp_buf[LED_WY/2+y+1] = 0;
		disp_buf[LED_WY/2-y-1] = 0;
		
	}
}



void row0_set(u8 set)
{
	P40 = set;
}
void row1_set(u8 set)
{
	P41 = set;
}
void row2_set(u8 set)
{
	P42 = set;
}
void row3_set(u8 set)
{
	P43 = set;
}
void row4_set(u8 set)
{
	P44 = set;
}
void row5_set(u8 set)
{
	P00 = set;
}
void row6_set(u8 set)
{
	P01 = set;
}
void row7_set(u8 set)
{
	P02 = set;
}
void row8_set(u8 set)
{
	P03 = set;
}
void row9_set(u8 set)
{
	P04 = set;
}
void row10_set(u8 set)
{
	P05 = set;
}
void row11_set(u8 set)
{
	P06 = set;
}
void row12_set(u8 set)
{
	P07 = set;
}
void row13_set(u8 set)
{
	P10 = set;
}
void row14_set(u8 set)
{
	P11 = set;
}
void row15_set(u8 set)
{
	P47 = set;
}
void row16_set(u8 set)
{
	P12 = set;
}
void row17_set(u8 set)
{
	P13 = set;
}
void row18_set(u8 set)
{
	P14 = set;
}
void row19_set(u8 set)
{
	P15 = set;
}
void row20_set(u8 set)
{
	P16 = set;
}

struct {
	void (code *func_ptr) (u8 set);
} pio_set[21] = {{row0_set},{row1_set},{row2_set},{row3_set},{row4_set},{row5_set},{row6_set},
			{row7_set},{row8_set},{row9_set},{row10_set},{row11_set},{row12_set},{row13_set},
			{row14_set},{row15_set},{row16_set},{row17_set},{row18_set},{row19_set},{row20_set}};


u8 row_curr = 0;
u8 row_prev = 20;
void led_scan(void) 
{
	u8 byte1;
	//u8 byte2;
	//关闭显示
	pio_set[row_prev].func_ptr(1);
	
	byte1 = disp_buf[row_curr];

	//显示下一个字符，板子做反了。
	P2 = 0;
	P45 = 0;
	P46 = 0;
	if(byte1 & 0x1)P24 = 1;
	if(byte1 & 0x2)P23 = 1;
	if(byte1 & 0x4)P22 = 1;
	if(byte1 & 0x8)P21 = 1;
	if(byte1 & 0x10)P20 = 1;
	if(byte1 & 0x20)P46 = 1;
	if(byte1 & 0x40)P45 = 1;
	if(byte1 & 0x80)P27 = 1;
	byte1 = disp_buf[row_curr] >> 8;
	if(byte1 & 0x1)P26 = 1;
    if(byte1 & 0x2)P25 = 1;


	//开启显示
	pio_set[row_curr].func_ptr(0);

	row_prev = row_curr;
	row_curr++;
	if(row_curr >= 21)
	row_curr= 0;
}









