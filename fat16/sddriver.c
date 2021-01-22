/*******************************************************************************************************
** Descriptions:		sd 卡驱动软件包: SD卡物理层 用户API函数
********************************************************************************************************/
#include "sddriver.h"

extern bool	SanDisk;

/* SD卡信息结构体变量 the information structure variable of SD Card */
sd_struct sds;

/* 超时时间单位表(单位:0.000000001ns) timeout unit table */
const INT32U time_unit[8] = {1000000000,100000000,10000000,1000000,100000,10000,1000,100};

/* 超时时间表 timeout value table */
const INT8U time_value[16] = {0,10,12,13,15,20,25,30,35,40,45,50,55,60,70,80};

/* 超时时间因数表 timeout factor table */
const INT8U r2w_fator[6] = {1,2,4,8,16,32};

/********************************************************************************************

用户API函数:  初始化,读,写,擦 SD卡  User API Function: Initialize,read,write,erase SD Card

********************************************************************************************/


/*******************************************************************************************************************
** 函数名称: INT8U SD_Initialize()				Name:	  INT8U SD_Initialize()
** 功能描述: 初始化SD卡							Function: initialize sd card
** 输　  入: 无									Input:	  NULL
** 输 　 出: 0:   正确    >0:   错误码		  	Output:	  0:  right		>0:  error code
********************************************************************************************************************/
INT8U SD_Initialize(void)
{
	INT8U recbuf[4],ret;

	//test
	DelayX(100);
	
	SD_HardWareInit();					    		/* 初始化读写SD卡的硬件条件 Initialize the hardware that access SD Card */

	SPI_Clk400k();
	
	SD_StartSD();									/* 向OS申请访问卡信号量 request semaphore acessed SD/MMC to OS */
	
    if (SD_ChkCard() != 0)
    {
    	SD_EndSD();
//    	printf("card is not inserted entirely!\n");
    	return SD_ERR_NO_CARD;   					/* 卡没完全插入卡中 card is not inserted entirely */
	}

	SPI_CS_Assert();								/* 1. 置CS为低 assert CS */

	//test
	DelayX(30);
//	SD_SPIDelay(30);								/* 2. 至少延时 74 clock delay more than 74 clock */
	SPI_CS_Deassert();								/* 3. 置CS为高 dessert CS */

	//test
	DelayX(8);
//	SD_SPIDelay(8);									/* 4. 延时2(8 clock) delay 2(8 clock) */
	SPI_CS_Assert();

	ret = SD_ResetSD();								/* 5. 发出CMDO命令复位SD卡 send CMD0 command to reset sd card */

//	printf("SD Reset ret=0x%x\n",ret);

    if (ret != SD_NO_ERR)
        return ret;

 	ret = SD_ActiveInit();							/* 6. 激活卡进入初始化过程. active card initialize process */

//	printf("SD ActiveInit ret=0x%x\n",ret);

 	if (ret != SD_NO_ERR)
 		return ret;

   	ret = SD_ReadOCR(4, recbuf);  					/* 7. 读OCR寄存器,查询卡支持的电压值 read OCR register,get the supported voltage */

//	printf("SD Read OCR ret=0x%x\n",ret);
//	printf("OCR[]=%x %x %x %x\n",recbuf[0],recbuf[1],recbuf[2],recbuf[3]);

	if (ret != SD_NO_ERR)
	    return ret;
	
	if ((recbuf[1] & MSK_OCR_33) != MSK_OCR_33)
	{
		printf("do not support 3.3V\n");
	    return SD_ERR_VOL_NOTSUSP;					/* 不支持3.3V,返回错误码  not support 3.3V,return error code */
	}

    SPI_ClkToMax();									/* 8. 设置SPI时钟到最大值 set SPI clock to maximum */

	#if SD_CRC_EN
	    ret = SD_EnableCRC(1);						/* 使能CRC校验 enable CRC check */

//		printf("SD Enable CRC ret=0x%x\n",ret);

		if (ret != SD_NO_ERR)
		  	return ret;
	#endif

    ret = SD_SetBlockLen(SD_BLOCKSIZE);				/* 9. 设置块的长度: 512Bytes Set the block length: 512Bytes */

//	printf("SD SetBlockLen ret=0x%x\n",ret);

	if (ret != SD_NO_ERR)
	    return ret;

	ret=SD_GetCardInfo();							/* 10. 读CSD寄存器,获取SD卡信息 read CSD register, get the information of SD card */
	
	SD_EndSD();										/* 归还访问卡信号量 return semaphore acessed SD/MMC to OS */
	
	SPI_ClkToMax();
	
	return ret;
}

/********************************************************************************************************************
** 函数名称: INT8U SD_ReadBlock()					Name:	  INT8U SD_ReadBlock()
** 功能描述: 从SD/MMC卡中读一个块					Function: read a single block from SD/MMC card
** 输　  入: INT32U blockaddr: 块地址				Input:    INT32U blockaddr: address of block
			 INT8U *recbuf   : 接收缓冲区,长度512Bytes	 	  INT8U *recbuf   : the buffer of receive,length is 512Bytes
** 输    出: 0:   成功    >0:  错误码				Output:	  0:  right			>0:  error code
*********************************************************************************************************************/
INT8U SD_ReadBlock(INT32U blockaddr, INT8U *recbuf)
{
	INT8U ret;

    if (SD_ChkCard() != 0)
    {
    	SD_EndSD();
    	printf("card is not inserted entirely\n");
    	return SD_ERR_NO_CARD;   					/* 卡没完全插入卡中 card is not inserted entirely */
	}

	if (blockaddr > sds.block_num)
	{
		SD_EndSD();
		printf("SD ReadBlock\nblockaddr=0x%x sds.block_num=0x%x\n",blockaddr,sds.block_num);
		printf("operate over the card range\n");
		return SD_ERR_OVER_CARDRANGE;				/* 操作超出卡容量范围 operate over the card range */
	}

 	ret = SD_ReadSingleBlock(blockaddr);			/* 读单块命令 read single blocks command */
 	if (ret != SD_NO_ERR)
 	{
 		printf("SD Read Single Block Address Error!\n");
 		SD_EndSD();
 		return ret;
 	}

  	ret = SD_ReadBlockData(SD_BLOCKSIZE, recbuf);	/* 读出数据 read data from sd card */
	SD_EndSD();										/* 归还访问卡信号量 return semaphore acessed SD/MMC to OS */

	return ret;
}

/********************************************************************************************************************
** 函数名称: INT8U SD_ReadMultiBlock()				Name:	  INT8U SD_ReadMultiBlock()
** 功能描述: 从SD/MMC卡中读多个块					Function: read multi blocks from SD/MMC card
** 输　  入: INT32U blockaddr: 块地址				Input:	  INT32U blockaddr: address of block
			 INT32U blocknum : 块数量						  INT32U blocknum : the numbers of block
			 INT8U *recbuf   : 接收缓冲区,每块512字节		  INT8U *recbuf   : the buffer of receive,each block length is 512Bytes
** 输    出: 0:   成功    >0:  错误码				Output:	  0:  right			>0:  error code
*********************************************************************************************************************/
#if SD_ReadMultiBlock_EN
INT8U SD_ReadMultiBlock(INT32U blockaddr, INT32U blocknum, INT8U *recbuf)
{
    INT32U i;
    INT8U ret;

//	printf("SD ReadMultiBlock Start!\n");
	
    if (SD_ChkCard() != 0)
    {
    	SD_EndSD();
    	printf("card is not inserted entirely\n");
    	return SD_ERR_NO_CARD;   					/* 卡没完全插入卡中 card is not inserted entirely */
	}

	if ((blockaddr + blocknum) > sds.block_num)
	{
		SD_EndSD(); 
		printf("blockaddr=0x%x\nblocknum=0x%x sds.block_num=0x%x\n",blockaddr,blocknum,sds.block_num);
		printf("operate over the card range\n");
		return SD_ERR_OVER_CARDRANGE;				/* 操作超出卡容量范围 operate over the card range */
	}

	ret = SD_ReadMultipleBlock(blockaddr);			/* 读多块命令 read multiple blocks command */
	if (ret != SD_NO_ERR)
	{
		SD_EndSD();									/* 归还访问卡信号量 return semaphore acessed SD/MMC to OS */
		return ret;
    }

    for (i = 0; i < blocknum; i++)
    {												/* 读出数据 read data from SD/MMC card */
    	ret = SD_ReadBlockData(SD_BLOCKSIZE, recbuf);
    	if (ret == SD_NO_ERR)
    	{
       		recbuf = recbuf + SD_BLOCKSIZE;
      	}	
    	else
    	{
    		SD_EndSD();
    		printf("read data error\n");
    		return ret;
    	}
    }

 	ret = SD_StopTransmission();				    /* 结束数据传输 stop transmission operation */

 	SD_EndSD();
 	
//	printf("SD ReadMultiBlock Done!\n");
	return ret;
}
/********************************************************************************************************************
** 函数名称: INT8U SD_ReadMultiBlockSanDisk()				Name:	  INT8U SD_ReadMultiBlockSanDisk()
** 功能描述: 从SD/MMC卡中读多个块					Function: read multi blocks from SD/MMC card
** 输　  入: INT32U blockaddr: 块地址				Input:	  INT32U blockaddr: address of block
			 INT32U blocknum : 块数量						  INT32U blocknum : the numbers of block
			 INT8U *recbuf   : 接收缓冲区,每块512字节		  INT8U *recbuf   : the buffer of receive,each block length is 512Bytes
** 输    出: 0:   成功    >0:  错误码				Output:	  0:  right			>0:  error code
*********************************************************************************************************************/
INT8U SD_ReadMultiBlockSanDisk(INT32U blockaddr, INT32U blocknum, INT8U *recbuf)
{
    INT32U i;
    INT8U ret;

//	printf("SanDisk ReadMultiBlock\n");
	
    if (SD_ChkCard() != 0)
    {
    	SD_EndSD();
    	printf("card is not inserted entirely\n");
    	return SD_ERR_NO_CARD;   					/* 卡没完全插入卡中 card is not inserted entirely */
	}

	if ((blockaddr + blocknum) > sds.block_num)
	{
		SD_EndSD(); 
		printf("blocknum=0x%x sds.block_num=0x%x\n",blocknum,sds.block_num);
		printf("operate over the card range\n");
		return SD_ERR_OVER_CARDRANGE;				/* 操作超出卡容量范围 operate over the card range */
	}

    for (i = 0; i < blocknum; i++)
    {												/* 读出数据 read data from SD/MMC card */
		ret=SD_ReadBlock(blockaddr,recbuf);
    	if (ret == SD_NO_ERR)
    	{
    		blockaddr++;
       		recbuf = recbuf + SD_BLOCKSIZE;
    	}
    	else
    	{
    		SD_EndSD();
    		printf("read data error\n");
    		return ret;
    	}
    }

// 	ret = SD_StopTransmission();				    /* 结束数据传输 stop transmission operation */

 	SD_EndSD();
 	
//	printf("SD ReadMultiBlock Done!\n");
	return ret;
}
#endif

/********************************************************************************************************************
** 函数名称: INT8U SD_WriteBlock()					Name:	  INT8U SD_WriteBlock()
** 功能描述: 向SD/MMC卡中写入一个块					Function: write a block to SD/MMC card
** 输　  入: INT32U blockaddr: 块地址				Input: 	  INT32U blockaddr: address of block
			 INT8U *sendbuf  : 发送缓冲区,长度512Bytes	  	  INT8U *sendbuf  : the buffer of send,length is 512Bytes
** 输    出: 0:   成功    >0:  错误码				Output:	  0:  right			>0:  error code
*********************************************************************************************************************/
INT8U SD_WriteBlock(INT32U blockaddr, INT8U *sendbuf)
{
	INT8U ret,tmp[2];

    if (SD_ChkCard() != 0)
    {
    	SD_EndSD();
    	printf("card is not inserted entirely\n");
    	return SD_ERR_NO_CARD;   								/* 卡没完全插入卡中 card is not inserted entirely */
	}

	if (blockaddr > sds.block_num)
	{
		SD_EndSD();
		printf("operate over the card range\n");
		return SD_ERR_OVER_CARDRANGE;							/* 操作超出卡容量范围 operate over the card range */
	}

	if (SD_ChkCardWP() != 0)
	{
		SD_EndSD();
    	printf("\ncard write protect\n");		
		return SD_ERR_WRITE_PROTECT;							/* 卡有写保护 card write protect*/
	}

	ret = SD_WriteSingleBlock(blockaddr);						/* 写单块命令 write single block */
	if (ret != SD_NO_ERR)
	{
		SD_EndSD();
		printf("SD Write Single Block Address Error!\n");
		return ret;
	}

	ret = SD_WriteBlockData(0, SD_BLOCKSIZE, sendbuf);			/* 写入数据 write data */
 	if (ret == SD_NO_ERR)										/* 读Card Status寄存器, 检查写入是否成功 */
 	{															/* read Card Status register to check write wheather sucessfully */
 		ret = SD_ReadCard_Status(2, tmp);
 		if (ret != SD_NO_ERR)
 		{
 			SD_EndSD();
 			printf("read register fail\n");
 			return ret;											/* 读寄存器失败 read register fail */
		}

 		if((tmp[0] != 0) || (tmp[1] != 0))
 		{
 			SD_EndSD();
			ret = SD_ERR_WRITE_BLK; 			     			/* 响应指示写失败 response indicate write fail */
 		}
 	}

    SD_EndSD();
 	return ret;													/* 返回写入结果 return the result of writing */
}

/**********************************************************************************************************************
** 函数名称: INT8U SD_WriteMultiBlock()				Name:	  INT8U SD_WriteMultiBlock()
** 功能描述: 向SD/MMC卡中写入多个块					Function: write multi blocks to SD/MMC card
** 输　  入: INT32U blockaddr: 块地址				Input:	  INT32U blockaddr: address of block
			 INT32U blocknum : 块数量						  INT32U blocknum : the numbers of block
			 INT8U *sendbuf  : 发送缓冲区每块512字节    	  INT8U *sendbuf  : the send buffer,each block length is 512Bytes
** 输    出: 0:   成功    >0:  错误码				Output:	  0:  right			>0:  error code
***********************************************************************************************************************/
#if SD_WriteMultiBlock_EN
INT8U SD_WriteMultiBlock(INT32U blockaddr, INT32U blocknum, INT8U *sendbuf)
{
	INT32U i;
	INT8U ret,tmp[2];

//	printf("SD WriteMultiBlock Start!\n");
//	printf("\n\n\n\nblockaddr=0x%x SD WriteMultiBlock blocknum=%d\n",blockaddr,blocknum);

    if (SD_ChkCard() != 0)
    {
    	SD_EndSD();
    	printf("card is not inserted entirely\n");
    	return SD_ERR_NO_CARD;   								/* 卡没完全插入卡中 card is not inserted entirely */
	}

	if ((blockaddr + blocknum) > sds.block_num)
	{
		SD_EndSD();
		printf("operate over the card range\n");
		return SD_ERR_OVER_CARDRANGE;							/* 操作超出卡容量范围 operate over the card range */
	}

	if (SD_ChkCardWP() != 0)
	{
		SD_EndSD();
    	printf("\ncard write protect\n");		
		return SD_ERR_WRITE_PROTECT;							/* 卡有写保护 card write protect*/
	}

	ret = SD_WriteMultipleBlock(blockaddr);						/* 写多块命令 write multiple blocks command */
	if (ret != SD_NO_ERR)
	{
		printf("SD Write MultipleBlock Address Error!\n");
		SD_EndSD();
		return ret;
	}

	
    for (i = 0; i < blocknum; i++)
    {
 		ret = SD_WriteBlockData(1, SD_BLOCKSIZE, sendbuf);		/* 写入数据 write data */
 		if (ret == SD_NO_ERR)
 			sendbuf = sendbuf + SD_BLOCKSIZE;
 		else
 		{														/* 写失败 write fail */
			SD_StopTransmission();								/* 停止数据传输 stop data transmission */
			SD_WaitBusy(SD_WAIT_WRITE);							/* 等待 waiting */
			SD_EndSD();
			return ret;
		}
	}

    SD_StopMultiToken();										/* 发送数据停止令牌 send data stop token */

    ret = SD_WaitBusy(SD_WAIT_WRITE);							/* 等待写入的完成 wait for finishing writing */
//    if (ret != SD_NO_ERR)
//    {
//    	SD_EndSD();
//    	printf("wait for finishing writing\n\n");
//    	return SD_ERR_TIMEOUT_WRITE;
//    }
//
//    if (sds.card_type == CARDTYPE_SD)
//    {
//   		ret = SD_GetNumWRBlcoks(&i);							/* 读正确写入的块数 read the blocks that be written correctly */
//   		if (ret != SD_NO_ERR)
//   		{
//   			SD_EndSD();
//   			printf("read the blocks that be written correctly\n\n");
//   		  	return ret;
//   		}
//   		if(i != blocknum)
//   		{
//   			printf("the blocks that be written correctly is error\n\n");
//			ret =  SD_ERR_WRITE_BLKNUMS;						/* 正确写入块数错误 the blocks that be written correctly is error */
//		}
//   	}
//   	else
//   	{
//   	 	ret = SD_ReadCard_Status(2, tmp);
// 		if (ret != SD_NO_ERR)
// 		{
// 			SD_EndSD();
// 			printf("read register fail\n");
// 			return ret;											/* 读寄存器失败 read register fail */
//		}
// 		if((tmp[0] != 0) || (tmp[1] != 0))
// 		{
// 			printf("response indicate write fail\n");
//			ret = SD_ERR_WRITE_BLK; 			     			/* 响应指示写失败 response indicate write fail */
//		}
//   	}
//
   	SD_EndSD();
   	
//	printf("SD WriteMultiBlock Done!\n");
	return ret;													/* 返回写入成功 return write sucessfully */
}
#endif

/*********************************************************************************************************************
** 函数名称: INT8U SD_EraseBlock()					Name:	  INT8U SD_EraseBlock()
** 功能描述: 擦除SD/MMC卡中的块						Function: Erase the block of SD/MMC card
** 输　  入: INT32U startaddr: 起始地址				Input:    INT32U startaddr: start address
			 INT32U endaddr  : 终止地址						  INT32U endaddr  : end address
** 输    出: 0:   成功    >0:  错误码				Output:	  0:  right			>0:  error code
** 注    意: startaddr 和 blocknum 建议为 sds.erase_unit 的整数倍, 因为有的卡只能以 sds.erase_unit 为单位进行擦除
*********************************************************************************************************************/
#if SD_EraseBlock_EN
INT8U SD_EraseBlock(INT32U startaddr, INT32U blocknum)
{
	INT32 tmp;
	INT8U ret;

    if (SD_ChkCard() != 0)
    {
    	SD_EndSD();
    	printf("card is not inserted entirely\n");
    	return SD_ERR_NO_CARD;   								/* 卡没完全插入卡中 card is not inserted entirely */
	}

	if ((startaddr + blocknum) > sds.block_num)
	{
		SD_EndSD();
		printf("operate over the card range\n");
		return SD_ERR_OVER_CARDRANGE;							/* 操作超出卡容量范围 operate over the card range */
	}

	if (SD_ChkCardWP() != 0)
	{
		SD_EndSD();
    	printf("\ncard write protect\n");		
		return SD_ERR_WRITE_PROTECT;							/* 卡有写保护 card write protect*/
	}

	tmp = blocknum*512/sds.erase_unit;
	while(tmp >= 0)												/* 每次擦除扇区 once erase is sector size */
	{
		ret = SD_EraseStartBlock(startaddr);					/* 选择起始块地址 select start address */
		if (ret != SD_NO_ERR)
		{
			printf("tmp=%d\nSD Erase Start Block Address Error!\n",tmp);
			SD_EndSD();
			return ret;
		}

		ret = SD_EraseEndBlock(startaddr + sds.erase_unit - 1);		/* 选择终止块地址 select end address */
		if (ret != SD_NO_ERR)
		{
			printf("tmp=%d\nSD Erase End Block Address Error!\n",tmp);
			SD_EndSD();
			return ret;
		}

		ret = SD_EraseSelectedBlock();							/* 擦除所选的块 erase blocks selected */
		if (ret != SD_NO_ERR)
		{
			printf("tmp=%d\nSD Erase Selected Block Error!\n",tmp);
			SD_EndSD();
			return ret;
		}

		startaddr += sds.erase_unit;								/* 起始地址递增 */
//		blocknum  -= sds.erase_unit;
		tmp--;
//		tmp = blocknum - sds.erase_unit;
	}

	if (blocknum*512%sds.erase_unit > 0)											/* 擦除不够once_erase块 */
	{															/* erase blocks that numbers is not enough once_erase */
		ret = SD_EraseStartBlock(startaddr);
		if (ret != SD_NO_ERR)
		{
			printf("blocknum=%d\nSD Erase Start Block Error!\n",blocknum);
			SD_EndSD();
			return ret;
		}

		ret = SD_EraseEndBlock(startaddr);
		if (ret != SD_NO_ERR)
		{
			printf("blocknum=%d\nSD Erase End Block Error!\n",blocknum);
			SD_EndSD();
			return ret;
		}

		ret = SD_EraseSelectedBlock();
		if (ret != SD_NO_ERR)
		{
			printf("blocknum=%d\nSD Erase Selected Block Error!\n",blocknum);
			SD_EndSD();
			return ret;
		}
	}

	SD_EndSD();
	return SD_NO_ERR;											/* 返回擦除成功 return erase sucessfully */
}
#endif

/*******************************************************************************************************************
** 函数名称: INT8U SD_GetCardInfo()				Name:	  INT8U SD_GetCardInfo()
** 功能描述: 获得SD卡的信息						Function: get the information of SD card
** 输　  入: INT8U cardtype: 卡类型				Input:    INT8U cardtype: card type
** 输 　 出: 0:   正确    >0:   错误码		  	Output:	  0:  right		>0:  error code
*******************************************************************************************************************/
INT8U SD_GetCardInfo()
{
	INT32U tmp;
	INT8U cidbuf[16],csdbuf[16],ret,i;
	
	ret = SD_ReadCID(16,cidbuf);
	
//	printf("SD Read CID ret=%d\n",ret);
	
	if (ret != SD_NO_ERR)
		return ret;
	
	if(cidbuf[0]==0x03)
		SanDisk	=true;
		
	ret = SD_ReadCSD(16,csdbuf);	 								    		/* 读CSD寄存器    read CSD register */

//	printf("SD Read CSD ret=%d\n",ret);
	
	if (ret != SD_NO_ERR)
		return ret;
	
	SD_CalTimeout(csdbuf);														/* 计算超时时间值 calculate timeout value */

	/* 计算块的最大长度  */														/* calculate the size of a sector */
	sds.block_len = 1 << (csdbuf[READ_BL_LEN_POS] & READ_BL_LEN_MSK);  			/* (2 ^ READ_BL_LEN) */

	/* 计算卡中块的个数 */														/* calculate the sector numbers of the SD Card */
	sds.block_num = ((csdbuf[C_SIZE_POS1] & C_SIZE_MSK1) << 10) +
	      			 (csdbuf[C_SIZE_POS2] << 2) +
	 	 			((csdbuf[C_SIZE_POS3] & C_SIZE_MSK3) >> 6) + 1;				/* (C_SIZE + 1)*/

	tmp = ((csdbuf[C_SIZE_MULT_POS1] & C_SIZE_MULT_MSK1) << 1) +
	      ((csdbuf[C_SIZE_MULT_POS2] & C_SIZE_MULT_MSK2) >> 7) + 2;				/* (C_SIZE_MULT + 2) */

    /* 获得卡中块的数量 */														/* get the block numbers in card */
	sds.block_num = sds.block_num * (1 << tmp);									/* (C_SIZE + 1) * 2 ^ (C_SIZE_MULT + 2) */

	/* 计算扇区大小 */															/*calculate the size of sector */
	sds.erase_unit = ((csdbuf[SECTOR_SIZE_POS1] & SECTOR_SIZE_MSK1) << 1) +
	                 ((csdbuf[SECTOR_SIZE_POS2] & SECTOR_SIZE_MSK2) >> 7) + 1; /* SD (SECTOR_SIZE + 1) */

	#define GetCardCISD

	#ifdef GetCardCISD
		printf("CID[]=");
		for(i=0;i<16;i++)
			printf("%x ",cidbuf[i]);
		printf("\n");	
		printf("CSD[]=");
		for(i=0;i<16;i++)
			printf("%x ",csdbuf[i]);
		printf("\n");
	#endif

	#define GetCardInfo
	
	#ifdef GetCardInfo
		printf("block len=%d\n",sds.block_len);
		printf("card size mult=%d\n",1<<tmp);
		printf("block num=%d\n",sds.block_num);
		printf("memory capacity=%d\n",sds.block_num*sds.block_len);
		printf("card erase unit=%d\n\n\n",sds.erase_unit);
	#endif
	
	if(sds.block_len==1024)
		sds.block_num=sds.block_num*2;
	
	return SD_NO_ERR;															/* 返回执行成功 return perform sucessfully */
}

/*******************************************************************************************************************
** 函数名称: INT8U SD_CalTimeout()				Name:	  INT8U SD_CalTimeout()
** 功能描述: 计算读/写/擦超时时间				Function: get the information of SD card
** 输　  入: INT8U cardtype: 卡类型				Input:    INT8U cardtype: card type
			 INT8U *csdbuf : CSD寄存器内容		 	      INT8U *csdbuf : CSD register content
** 输 　 出: 0:   正确    >0:   错误码		  	Output:	  0:  right		>0:  error code
*******************************************************************************************************************/
void SD_CalTimeout(INT8U *csdbuf)
{
	INT32U tmp;
	INT8U time_u,time_v,fator;

	sds.timeout_read = READ_TIMEOUT_100MS;								/* 默认读超时为100ms */
	sds.timeout_write = WRITE_TIMEOUT_250MS;							/* 默认写超时为250ms */
	sds.timeout_erase = WRITE_TIMEOUT_250MS;

	time_u = (csdbuf[TAAC_POS] & TAAC_MSK);								/* 读超时时间单位 read timeout unit */
	time_v = (csdbuf[TAAC_POS] & NSAC_MSK) >> 3;						/* 读超时时间值   read timeout value */
	fator = (csdbuf[R2WFACTOR_POS] & R2WFACTOR_MSK) >> 2;				/* 写超时时间因数 write timeout factor */

	if(time_v == 0)	return;
	if(fator >= 6) return;

	tmp = SPI_CLOCK * time_value[time_v] / 10 / time_unit[time_u];		/* TACC * f (单位 unit: clock) */
	tmp = tmp + csdbuf[NSAC_POS] * 100;									/* TACC * f + NSAC * 100 (单位 unit: clock) */

	/* 计算得到的超时值 the timeout value of being calculated */
	sds.timeout_read = tmp;
	sds.timeout_write = tmp * r2w_fator[fator];							/* (TACC * f + NSAC * 100) * R2WFACTOR (单位 unit:clock)*/

	sds.timeout_read  = sds.timeout_read * 100 / 8;						/* 实际值为计算值的100倍 */
	sds.timeout_write = sds.timeout_write * 100 / 8;

	if (sds.timeout_read > READ_TIMEOUT_100MS)							/* 取计算值与默认值中的最小值 */
		sds.timeout_read = READ_TIMEOUT_100MS;

	if (sds.timeout_write > WRITE_TIMEOUT_250MS)
		sds.timeout_write = WRITE_TIMEOUT_250MS;

	sds.timeout_erase = sds.timeout_write;    
}

/*******************************************************************************************************************
** 函数名称: INT8U SD_ActiveInit()				Name:	  INT8U SD_ActiveInit()
** 功能描述: 计算读/写/擦超时时间				Function: get the Idle state of SD card
** 输　  入: void							 	Input:    void
** 输 　 出: 0:   正确    >0:   错误码		  	Output:	  0:  right		>0:  error code
** 函数说明: 该命令不断重复发送到SD卡，直到响应R1的Bit0(Idle)位为0，表示SD卡内部初始化处理完成。
		     当响应的Idle位为0时，SD卡就完全进入SPI模式了。当然重复发送命令CMD1是有次数限制的，
		     最大次数为宏定义SD_IDLE_WAIT_MAX.
*******************************************************************************************************************/
INT8U SD_ActiveInit(void)
{
	INT8U param[4] = {0,0,0,0},resp[5],ret;
	INT32U i = 0;

	do
	{													/* 发出CMD1, 查询卡的状态, send CMD1 to poll card status */
		ret = SD_SendCmd(CMD1, param, CMD1_R, resp);
		if (ret != SD_NO_ERR)
			return ret;

		i++;
		
		//test
		DelayX(10);
//		printf("CMD1 resp[0]=0x%x\n",resp[0]);
//		SD_SPIDelay(10);
	}
	while (((resp[0] & MSK_IDLE) == MSK_IDLE) && (i < SD_IDLE_WAIT_MAX)); 		/* 如果响应R1的最低位Idle位为1,则继续循环 */
	
	if (i >= SD_IDLE_WAIT_MAX)
		return SD_ERR_TIMEOUT_WAITIDLE;					/* 超时,返回错误 time out,return error */

    ret = SD_SendCmd(CMD55, param, CMD55_R, resp);

//	printf("SD ActiveInit CMD55 ret=0x%x\n",ret);

	if(ret != SD_NO_ERR)
		return ret;

    ret = SD_SendCmd(ACMD41, param, ACMD41_R, resp);		/* 激活内部初始化命令 active card to initialize process internal */

//	printf("SD ActiveInit ACMD41 ret=0x%x\n",ret);

	if (ret != SD_NO_ERR)
		return SD_ERR_UNKNOWN_CARD;
	
	if ((resp[0] & 0xFE) == 0)
	{
	  	sds.card_type = CARDTYPE_SD;						/* 是SD卡 the card is SD card */
	  	printf("the card is SD card\n");
	}
	else
	{
		sds.card_type = CARDTYPE_MMC;						/* 是MMC卡 the card is MMC card */
		printf("the card is MMC card\n");
	}

	return SD_NO_ERR;
}

/*******************************************************************************************************************
** 函数名称: void SD_StartSD()					Name:	  INT8U SD_StartSD()
** 功能描述: 向操作系统申请访问SD卡的权限		Function: request the right of operating sd to OS
** 输　  入: 无									Input:	  NULL
** 返	 回: 无									return:	  NULL
********************************************************************************************************************/
void SD_StartSD(void)
{
	#if SD_UCOSII_EN
		INT8U ret;

		OSSemPend(pSemSD, 0, &ret);					/* 等待访问卡信号量可用 wait for semaphore that accessed Card */
	#endif
}

/*******************************************************************************************************************
** 函数名称: void SD_EndSD()					Name:	  INT8U SD_EndSD()
** 功能描述: 访问SD卡的权限归还操作系统			Function: return the right of operating sd to OS
** 输　  入: 无									Input:	  NULL
** 返	 回: 无									return:	  NULL
********************************************************************************************************************/
void SD_EndSD(void)
{
	#if SD_UCOSII_EN
		OSSemPost(pSemSD);							/* 将访问卡信号量还给操作系统 return the semaphore accessing Card to OS */
	#endif
}
