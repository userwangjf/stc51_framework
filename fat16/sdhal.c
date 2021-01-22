/*****************************************************************************************************
** Descriptions:sd 卡驱动软件包: 硬件抽象层 ---- SPI操作函数
********************************************************************************************************/
#include "sdhal.h"

// SPI transfer mode
#define TIMOD_RX		0x00
#define TIMOD_TX 		0x01
#define TIMOD_DMA_RX 	0x02
#define TIMOD_DMA_TX 	0x03
#define TIMOD_MASK 		0x03

	/**************************************************************

		读写SD卡的SPI接口函数: SPI接口设置,发送/接收字节函数

	**************************************************************/
/*******************************************************************************************************************
** 函数名称: void SD_Power()					Name:	  void SD_Power()
** 功能描述: 对卡先下电,再上电					Function: turn off the card's power, and turn on
** 输　  入: 无									Input:	  NULL
** 输 　 出: 无									Output:	  NULL
********************************************************************************************************************/
void SD_Power(void)
{
//	INT32U i;
//
//	SD_POWER_GPIO();
//	SD_POWER_OUT();
//	SD_POWER_OFF();								/* 关闭 SD 卡电源  turn off power of sd card */
//	
//	SPI_SCK_GPIO();	
//	SPI_SCK_OUT();
//	SPI_SCK_CLR();								/* SCK  引脚置低   set SCK to zero */
//	
//	SPI_MISO_GPIO();
//	SPI_MISO_OUT();
//	SPI_MISO_CLR();								/* MISO 引脚置低   set MISO to zero */
//	
//	SPI_MOSI_GPIO();		
//	SPI_MOSI_OUT();
//	SPI_MOSI_CLR();								/* MOSI 引脚置低   set MOSI to zero */
//	
//	SPI_CS_GPIO();								
//   	SPI_CS_OUT();								
//	SPI_CS_CLR();								/* CS 引脚置低	   set CS to zero */
//		
//	for(i = 0; i < 0x9000; i++);				/* 关闭电源延时    delay after turn off power of sd card */
//	SD_POWER_ON();								/* 打开 SD 卡电源  turn on power of sd card */
}

/********************************************************************************************************
** 函数名称：SPI_Init()
** 函数功能：将SSP控制器设置为主机SPI。
** 入口参数：无
** 出口参数：无
**********************************************************************************************************/
void  SPI_INIT(void)
{
	*pSPI_FLG	=0xFF10;		//PF4 as Chip Select
	*pSPI_BAUD	=0x0004;
	*pSPI_CTL	=0x5525;		//Enable SPI,Master,Enable MISO
}


/*******************************************************************************************************************
** 函数名称: void SD_HardWareInit()				Name:	  void SD_HardWareInit()
** 功能描述: 初始化访问SD卡的硬件条件			Function: initialize the hardware condiction that access sd card
** 输　  入: 无									Input:	  NULL
** 输 　 出: 无									Output:	  NULL
********************************************************************************************************************/
void SD_HardWareInit(void)
{
	SPI_INIT();									/* 初始化SPI接口 */
	SPI_CS_SET();								/* CS置高 */
}


/*******************************************************************************************************************
** 函数名称: void SPI_Clk400k()					Name:	  void SPI_Clk400k()
** 功能描述: 设置SPI的时钟小于400kHZ			Function: set the clock of SPI less than 400kHZ
** 输　  入: 无									Input:	  NULL
** 输 　 出: 无									Output:	  NULL
********************************************************************************************************************/
void SPI_Clk400k(void)
{
//	*pSPI_BAUD	=0x00A1;   						/* 设置SPI时钟分频值为161  Set the value of dividing frequency to 322 */
	*pSPI_BAUD	=0x00DE;   						/* 设置SPI时钟分频值为222  Set the value of dividing frequency to 444 */
}


/*******************************************************************************************************************
** 函数名称: void SPI_ClkToMax()				Name:	  void SPI_ClkToMax()
** 功能描述: 设置SPI的clock到最大值				Function: set the clock of SPI to maximum
** 输　  入: 无									Input:	  NULL
** 输 　 出: 无									Output:	  NULL
********************************************************************************************************************/
void SPI_ClkToMax(void)
{
	*pSPI_BAUD	=0x0005;					/* 设置SPI时钟分频值为2*3  Set the value of dividing frequency to 2*3 */
}


/*******************************************************************************************************************
** 函数名称: void SPI_SendByte()				Name:	  void SPI_SendByte()
** 功能描述: 通过SPI接口发送一个字节			Function: send a byte by SPI interface
** 输　  入: INT8U byte: 发送的字节				Input:	  INT8U byte: the byte that will be send
** 输 　 出: 无									Output:	  NULL
********************************************************************************************************************/
void SPI_SendByte(INT8U byte)
{
	*pSPI_STAT = TXCOL | RBSY | MODF | TXE;
	*pSPI_CTL =MSTR | TIMOD_TX | SPE;
	*pSPI_TDBR = byte;

	while(*pSPI_STAT & TXS);
	while(((*pSPI_STAT & SPIF)==0));
}


/*******************************************************************************************************************
** 函数名称: INT8U SPI_RecByte()				Name:	  INT8U SPI_RecByte()
** 功能描述: 从SPI接口接收一个字节				Function: receive a byte from SPI interface
** 输　  入: 无									Input:	  NULL
** 输 　 出: 收到的字节							Output:	  the byte that be received
********************************************************************************************************************/
INT8U SPI_RecByte(void)
{
	*pSPI_STAT = TXCOL | RBSY | MODF | TXE;
	*pSPI_CTL =MSTR | TIMOD_TX | SPE ;
	*pSPI_TDBR = 0xFF;

	while(*pSPI_STAT & TXS);
	while(((*pSPI_STAT & SPIF)==0));

	return (unsigned char)*pSPI_RDBR;
}


/*******************************************************************************************************************
** 函数名称: void SPI_CS_Assert()				Name:	  void SPI_CS_Assert()
** 功能描述: 片选SPI从机						Function: select the SPI slave
** 输　  入: 无									Input:	  NULL
** 输 　 出: 无									Output:	  NULL
********************************************************************************************************************/
void SPI_CS_Assert(void)
{
	SPI_CS_CLR();			   					/* 片选SPI从机  select the SPI slave */
}


/*******************************************************************************************************************
** 函数名称: void SPI_CS_Deassert()				Name:	  void SPI_CS_Deassert()
** 功能描述: 不片选SPI从机						Function: not select the SPI slave
** 输　  入: 无									Input:	  NULL
** 输 　 出: 无									Output:	  NULL
********************************************************************************************************************/
void SPI_CS_Deassert(void)
{
	SPI_CS_SET();			    				/* 不片选SPI从机  not select the SPI slave */
}


/*******************************************************************************************************************
** 函数名称: void SPI_CS_CLR()					Name:	  SPI_CS_CLR()
** 功能描述: 片选SPI从机						Function: select the SPI slave
** 输　  入: 无									Input:	  NULL
** 输 　 出: 无									Output:	  NULL
********************************************************************************************************************/
void SPI_CS_CLR(void)
{
	*pSPI_FLG	=0xFF10;
}


/*******************************************************************************************************************
** 函数名称: void SPI_CS_SET()					Name:	  void SPI_CS_SET()
** 功能描述: 不片选SPI从机						Function: not select the SPI slave
** 输　  入: 无									Input:	  NULL
** 输 　 出: 无									Output:	  NULL
********************************************************************************************************************/
void SPI_CS_SET(void)
{
	*pSPI_FLG	=0xEF10;
}

/*******************************************************************************************************************
** 函数名称: void SD_ChkCard()					Name:	  void SD_ChkCard()
** 功能描述: 检测卡是否完全插入					Function: check weather card is insert entirely
** 输　  入: 无									Input:	  NULL
** 输 　 出: 0: 卡完全插入	1: 卡没有完全插入   Output:	  1: insert entirely	0: not insert entirely
********************************************************************************************************************/
INT8U SD_ChkCard(void)
{
	if (SD_INSERT_STATUS() == PF10)
		return SD_ERR_NO_CARD;					/* 未完全插入 not insert entirely */
	else
		return SD_NO_ERR;						/* 完全插入 insert entirely */
}

/*******************************************************************************************************************
** 函数名称: void SD_ChkCardWP()				Name:	  void SD_ChkCardWP()
** 功能描述: 检测卡写保护						Function: check weather card is write protect
** 输　  入: 无									Input:	  NULL
** 输 　 出: 1: 卡已写保护	0: 卡未写保护	    Output:	  1: insert write protect	0: not write protect
********************************************************************************************************************/
INT8U SD_ChkCardWP(void)
{
	if (SD_WP_STATUS() == PF9)
		return SD_ERR_WRITE_PROTECT;			/* 卡写保护 */
	else
		return SD_NO_ERR;						/* 卡未写保护 */
}
