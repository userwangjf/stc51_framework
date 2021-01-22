

#include "bsp/config.h"
#include "ds3231/ds3231.h"

//26,31,21,06,16,05,26,
//00,00,00,00,00,00,10,
//1c,8a,00,15,40,

stSysTime ds3231_time;


#define I2Ca_ADDR	0xd0

#define SCL1	P54 = 1
#define	SCL0	P54 = 0
#define SDA1 	P55 = 1
#define SDA0	P55 = 0
#define SDA		P55

//定义I2C序号
#define I2C()  I2C ## a

#define I2C_Delay()	delay_us(5)

#include "io_i2c/io_i2c.h"


void ds3231_write(u8 addr, u8 byte)
{
   	I2Ca_Start();
	I2Ca_SendByte(I2Ca_ADDR&0xfe);
	I2Ca_RecvAck();
	I2Ca_SendByte(addr);
	I2Ca_RecvAck();
	I2Ca_SendByte(byte);
	I2Ca_RecvAck();
	I2Ca_Stop();
}


u8 ds3231_read(u8 addr)
{
	u8 rdata;

	//先写入地址，不带数据。
   	I2Ca_Start();
	I2Ca_SendByte(I2Ca_ADDR&0xfe);
	I2Ca_RecvAck();
	I2Ca_SendByte(addr);
	I2Ca_RecvAck();
	I2Ca_Stop();

	//然后读一个字节。
	I2Ca_Start();
	I2Ca_SendByte(I2Ca_ADDR|0x01);
	I2Ca_RecvAck();
	rdata = I2Ca_RecvByte();
	I2Ca_SendAck(1);
	I2Ca_Stop();
	
	return rdata;	
}

//bcd码转换为十进制
u8 bcd2dec(u8 bcd) {
	return (bcd & 0xf) + ((bcd >> 4) * 10);
}

void ds3231_updateTime(void)
{
	u8 tmp;

	tmp = 0;

repeat_read:

	ds3231_time.second = bcd2dec(ds3231_read(DS3231_SECOND));
	ds3231_time.minute = bcd2dec(ds3231_read(DS3231_MINUTE));
	ds3231_time.hour   = bcd2dec(ds3231_read(DS3231_HOUR));
	ds3231_time.week   = bcd2dec(ds3231_read(DS3231_WEEK));
	ds3231_time.day    = bcd2dec(ds3231_read(DS3231_DAY));
	ds3231_time.month  = bcd2dec(ds3231_read(DS3231_MONTH));
	ds3231_time.year   = bcd2dec(ds3231_read(DS3231_YEAR));
	
	if(tmp)return;

	//秒翻转，则需要重读
	tmp = ds3231_read(DS3231_SECOND);
	if(ds3231_time.second > tmp)
	{
		tmp = 1;
		goto repeat_read;
	}
}

void ds3231_print_time()
{
	printk("\r\n%d-%d-%d %d:%d:%d",(u32)ds3231_time.year,(u32)ds3231_time.month,
	(u32)ds3231_time.day,(u32)ds3231_time.hour,(u32)ds3231_time.minute,(u32)ds3231_time.second);
}



bool ds3231_conv = 0;
//启动温度转换
void ds3231_startConv(void)
{

}

//获取温度值,放大100倍
u16 ds3231_getConv(void)
{
	u8 temph;
	u16 temp;
	u8 templ;
	//启动转换失败
	temph = ds3231_read(DS3231_TEMPH);
	templ = ds3231_read(DS3231_TEMPL);
	
	templ >>= 6;
	
	temp = (temph << 2) + templ;
	
	temp *= 25;
	
	printk("\r\n%d",(u32)temp);
	
	
	return temp;	
}
