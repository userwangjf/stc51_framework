

/*
//用法
//定义IO操作宏
#define SCL1	P25 = 1
#define	SCL0	P25 = 0
#define SDA1 	P24 = 1
#define SDA0	P24 = 0
#define SDA		P24

//定义I2C序号
#define I2C()  I2C ## 1

#define I2C_Delay()	delay_us(5)

#include "io_i2c/io_i2c.h"
*/

static void I2C()_Init(void)
{
	SDA1;
	SCL1;
}

/**************************************
起始信号
**************************************/
static void I2C()_Start(void)
{
    SDA1;                    //拉高数据线
    SCL1;                    //拉高时钟线
    I2C_Delay();          //延时
    SDA0;                    //产生下降沿
    I2C_Delay();          //延时
    SCL0;                    //拉低时钟线
	I2C_Delay();
}

/**************************************
停止信号
**************************************/
static void I2C()_Stop(void)
{
    SDA0;                    //拉低数据线
    SCL1;                    //拉高时钟线
    I2C_Delay();                 //延时
    SDA1;                    //产生上升沿
    I2C_Delay();                 //延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
static void I2C()_SendAck(bit ack)
{
    SDA = ack;                  //写应答信号
    SCL1;                    //拉高时钟线
    I2C_Delay();                 //延时
    SCL0;                    //拉低时钟线
    I2C_Delay();                 //延时
}

/**************************************
接收应答信号
**************************************/
static bit I2C()_RecvAck(void)
{
	SDA1;
    SCL1;                    //拉高时钟线
    I2C_Delay();                 //延时
    CY = SDA;                   //读应答信号
    SCL0;                    //拉低时钟线
    I2C_Delay();                 //延时

    return CY;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
static void I2C()_SendByte(u8 dat)
{
    u8 i;

    for(i=0; i<8; i++)         //8位计数器
    {
		if(dat & 0x80)
			SDA1;
		else
			SDA0;
        dat <<= 1;              //移出数据的最高位
        SCL1;                //拉高时钟线
        I2C_Delay();             //延时
        SCL0;                //拉低时钟线
        I2C_Delay();             //延时
    }
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
static u8 I2C()_RecvByte(void)
{
    u8 i;
    u8 dat = 0;

    SDA1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL1;                //拉高时钟线
        I2C_Delay();        //延时
        dat |= SDA;             //读数据               
        SCL0;                //拉低时钟线
        I2C_Delay();             //延时
    }
    return dat;
}



