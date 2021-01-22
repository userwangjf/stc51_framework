
#ifndef		__CONFIG_H
#define		__CONFIG_H

//#define MAIN_Fosc		22118400L	//定义主时钟
#define MAIN_Fosc		12000000L	//定义主时钟
//#define MAIN_Fosc		11059200L	//定义主时钟
//#define MAIN_Fosc		 5529600L	//定义主时钟
//#define MAIN_Fosc		24000000L	//定义主时钟
#define Main_Fosc_KHZ	(MAIN_Fosc / 1000)

#include "STC15Fxxxx.H"


//STC内设驱动头文件
#include "bsp/type.h"
#include "bsp/gpio.h"
#include "bsp/delay.h"
#include "bsp/timer.h"
#include "bsp/usart.h"
#include "bsp/pca.h"
#include "bsp/exti.h"
#include "bsp/printk.h"
#include "bsp/bsp.h"

//操作系统头文件
#include "os/os_sch.h"

//调用应用程序文件
#include "app/app.h"
//功能模块
#include "module/module.h"

#endif
