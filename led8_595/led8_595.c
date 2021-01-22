//使用74LS595驱动的4个8位数码管

#include "led8_595/led8_595.h"


//数码管的亮度，共十级0~9
u8 led8_bright = 3;
//通过定时开关595的oe来控制数码管的亮度
//需要在定时中断函数里调用
u8 bright_curr = 0;
void led8_595_scan()
{
	if(bright_curr <= led8_bright)
		IO_595_OEn = 0;
	else
		IO_595_OEn = 1;

	if(bright_curr++ >= 9)
		bright_curr = 0;	
}

//设置数据管的亮度
void led8_bright_set(u8 bright)
{
	led8_bright = bright % 10;
}

//数据管可以显示字符的定义，共阳
u8 code seg_char[18] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xFF,0xbf};//0-9,A-F,灭,-

//使用595移动4个数据到数码管
void led8_595_dout(u8 d0, u8 d1, u8 d2, u8 d3)
{
	u8 i;

	for(i=0;i<8;i++) {
		IO_595_SI_0 = d0 >> 7;
		IO_595_SI_1 = d1 >> 7;
		IO_595_SI_2 = d2 >> 7;
		IO_595_SI_3 = d3 >> 7;
		IO_595_SCK = 0;
		IO_595_SCK = 0;
		IO_595_SCK = 1;
		IO_595_SCK = 1;
		d0 = d0 << 1;
		d1 = d1 << 1;
		d2 = d2 << 1;
		d3 = d3 << 1;
	}
	IO_595_LOAD = 0;
	IO_595_LOAD = 0;
	IO_595_LOAD = 1;
	IO_595_LOAD = 1;
}
	

//在led8上显示4个字符
void led8_595_num(u8 d0,u8 d1,u8 d2,u8 d3)
{
	led8_595_dout(seg_char[d0%16],seg_char[d1%16],seg_char[d2%16],seg_char[d3%16]);
}

//依次循环点亮字符0的6个段码，用于测试
void led8_595_loop_zero() {
	u8 i;
	u8 show;
	for(i=0;i<6;i++) {
		show = ~(0x1<<i);
		led8_595_dout(show,show,show,show);
		delay_ms(100);
	}
}

void led8_595_test() {
	u8 i,j;
	u32 cnt = 0;

	for(j=0;j<5;j++) {
		led8_595_loop_zero();
	}

	for(i=0;i<18;i++) {
		led8_595_dout(seg_char[i%18],seg_char[i%18],seg_char[i%18],seg_char[i%18]);
		os_delayTick(500000);
	}

}


