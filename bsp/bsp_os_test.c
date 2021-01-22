//本文件用于初始化单片机内部设备，外部需要调用bsp_init();

#include "bsp/config.h"


void GPIO_Config(void)
{
	GPIO_InitTypeDef	GPIO_Init;		//结构定义

	GPIO_Init.Pin  = GPIO_Pin_All;		//指定要初始化的IO, GPIO_Pin_0 ! GPIO_Pin_7, 或操作
	GPIO_Init.Mode = GPIO_OUT_PP;		//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP

	//初始化74HC595需要使用的IO
	GPIO_Inilize(GPIO_P2,&GPIO_Init);
	
	GPIO_Init.Pin = GPIO_Pin_7;
	GPIO_Inilize(GPIO_P3,&GPIO_Init);
	
	IO_595_RSTn = 0;
	IO_595_LOAD = 0;
	IO_595_OEn = 1;
	IO_595_SCK = 0;
	IO_595_POWER_OFF = 0;
	IO_595_RSTn = 1;
	IO_595_OEn = 0;


	//配置键盘列,P3.0~P3.5
	#if 0
	GPIO_Init.Pin = 0x3f;
	GPIO_Init.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P3,&GPIO_Init);
	#endif

}

//15W408，无Timer1.
//15W1K，有Timer1.
//先除12T，然后将16bit的计数器用满
//操作系统使用了timer的计数器作为低16bit。
void Timer_config(void)
{
	
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义

	#if 1
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 0x0;					//16bit全部使用
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2
	#endif

	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 0x10000 - 500;		//500us
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer1,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2

}


void UART_config(u8 int_en,u32 rate)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义

	//UART1
	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;		//模式,       UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;			//使用波特率,   BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = rate;		    	//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	if(int_en)
		COMx_InitStructure.UART_Interrupt = ENABLE;				//中断允许,   ENABLE或DISABLE
	else
		COMx_InitStructure.UART_Interrupt = DISABLE;
	COMx_InitStructure.UART_Polity    = PolityLow;			//中断优先级, PolityLow,PolityHigh
	COMx_InitStructure.UART_P_SW      = UART1_SW_P30_P31;	//切换端口,   UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17(必须使用内部时钟)
	COMx_InitStructure.UART_RXD_TXD_Short = DISABLE;		//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	USART_Configuration(USART1, &COMx_InitStructure);		//初始化串口1 USART1,USART2

	//UART2
	#if 0
	COMx_InitStructure.UART_Interrupt = DISABLE;
   	COMx_InitStructure.UART_P_SW = UART2_SW_P10_P11;
	USART_Configuration(USART2, &COMx_InitStructure);
	#endif
}

void PCA_config ( void )
{
	PCA_InitTypeDef		PCA_InitStructure;

	#if 1
	//配置作为高速PWM模式，用于8bit pcm数据播放
	PCA_InitStructure.PCA_Clock    = PCA_Clock_1T;		//PCA_Clock_1T, PCA_Clock_2T, PCA_Clock_4T, PCA_Clock_6T, PCA_Clock_8T, PCA_Clock_12T, PCA_Clock_Timer0_OF, PCA_Clock_ECI
	PCA_InitStructure.PCA_IoUse    = PCA_P34_P35_P36_P37;	//PCA_P12_P11_P10_P37, PCA_P34_P35_P36_P37, PCA_P24_P25_P26_P27
	PCA_InitStructure.PCA_Interrupt_Mode = DISABLE;		//ENABLE, DISABLE
	PCA_InitStructure.PCA_Polity   = PolityLow;			//优先级设置	PolityHigh,PolityLow
	//PCA_InitStructure.PCA_RUN      = DISABLE;			//ENABLE, DISABLE
	PCA_Init ( PCA_Counter,&PCA_InitStructure );

	PCA_InitStructure.PCA_Mode     = PCA_Mode_PWM;		//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
	PCA_InitStructure.PCA_PWM_Wide = 0;					//PCA_PWM_8bit, PCA_PWM_7bit, PCA_PWM_6bit
	PCA_InitStructure.PCA_Interrupt_Mode = DISABLE;		//PCA_Rise_Active, PCA_Fall_Active, ENABLE, DISABLE
	PCA_InitStructure.PCA_Value    = 0x8080;				//对于软件定时, 为匹配比较值

	PCA_Init ( PCA1,&PCA_InitStructure );

	#endif

}


//
/**
*外部中断初始化，使能鼠标的clk下降沿中断
*@brief
*@param
*@return

*/
void EXTI_config(void)
{
 	EXTI_InitTypeDef exti;

	exti.EXTI_Interrupt = ENABLE;
	exti.EXTI_Polity = PolityLow;
	exti.EXTI_Mode = EXT_MODE_RiseFall;

	//作为超声echo的接收
	Ext_Inilize(EXT_INT0,&exti);
}

//该任务在main函数里调用
void bsp_init()
{
	P0 = 0xff;
	P1 = 0xff;
	P2 = 0xff;
	P3 = 0xff;
	P4 = 0xff;
	P5 = 0xff;

	GPIO_Config();
	
	//PCA_config();
	//disp_clear();

	
	Timer_config();
	EA = 1;
   	UART_config(0,115200);
	printk("\r\nHelloUart1\r\n");
	
}


/********************* Timer0中断函数************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	EA = 0;
	os_incTick();
	EA = 1;
}

/********************* Timer1中断函数************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{
	EA = 0;
	led8_595_scan();
	EA = 1;
}


bool uart1_tx_rdy = 1;
u8* uart1_tx_ptr;
u8 uart1_echo_buf = 0;


#define S2RI 0x01
#define S2TI 0x02
void Uart2_Tx(u8* buf)
{
	for(;;)
	{
		if(*buf == 0)break;
		S2BUF = *buf++;
		while(!(S2CON & S2TI));
		S2CON &= ~S2TI;
	}
}

//发送时，仅拷贝指针，不拷贝数据。
void Uart1_Tx(u8 *string)
{
	//不能打断echo
	while(uart1_echo_buf != 0);
	//等待上一次传输完成
	#if UART_TX_WAIT
		//等上次传输完成。
		while(!uart1_tx_rdy);
	#else
		//发生溢出错误，打印~
		if(!uart1_tx_rdy)
		{
			ES = 0;
			while(!TI);
			SBUF = '~';
			while(!TI);
			ES = 1;
		}		
	#endif
	uart1_tx_rdy = 0;
	uart1_tx_ptr = string;
	while(uart1_echo_buf != 0);
	TI = 1;					//触发发送中断
}

void Uart1_Echo(u8 dat)
{
	uart1_echo_buf = dat;
	if(uart1_tx_rdy)
		TI = 1;
}

bool Uart1_Busy(void)
{
	if(uart1_tx_rdy)
		return 0;
	else
		return 1;
}

/********************* UART1中断函数************************/
void UART1_int (void) interrupt UART1_VECTOR
{
	u8 tmp;

	if(RI)
	{
		RI = 0;
		tmp = SBUF;		
		//shell_rx_dat(tmp);
	}

	if(TI)
	{
		TI = 0;
		//ECHO有最高优先级，插队了。
		if(uart1_echo_buf == 0xff)
		{
			uart1_echo_buf = 0;
			if(!uart1_tx_rdy)//上次发送未完成
				SBUF = *uart1_tx_ptr++;
		}
		else if(uart1_echo_buf != 0)
		{
			SBUF = uart1_echo_buf;
			uart1_echo_buf = 0xff;
		}
		else if(uart1_tx_rdy)
			;
		else if(*uart1_tx_ptr == 0)
			uart1_tx_rdy = 1;
		else
			SBUF = *uart1_tx_ptr++; 	
	}
}

/********************* UART2中断函数************************/
void UART2_int (void) interrupt UART2_VECTOR
{
	if(RI2)
	{
		CLR_RI2();

	}

	if(TI2)
	{
		CLR_TI2();
	}

}
