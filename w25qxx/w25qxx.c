

#include "bsp/config.h"
#include "w25qxx/w25qxx.h"
#include "w25qxx/w25qxxConfig.h"

#define W25QXX_DUMMY_BYTE         0xA5

w25qxx_t	w25qxx;

void W25qxx_Delay(u8 delay)	
{
	for(;delay>0;delay--);
}

//###################################################################################################################
u8 W25qxx_Spi(u8 dat)
{
	u8 ret;
	u8 i;

	ret = 0;
	for(i=0;i<8;i++)
	{
		ret <<= 1;
		if(dat&0x80)
			W25QXX_DI_SET;
		else
			W25QXX_DI_CLR;
		W25QXX_CLK_SET;
		//²ÉÑùÊä³ö
		if(W25QXX_DO_GET)
			ret |= 0x01;
		else
			ret &= 0xfe;
		W25QXX_CLK_CLR;
		dat <<= 1;
	}

	return ret;	
}

//###################################################################################################################
u32 W25qxx_ReadID(void)
{
	u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	W25QXX_CS_CLR;
	W25qxx_Spi(0x9F);
	Temp0 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
	Temp1 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
	Temp2 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
	W25QXX_CS_SET;
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
	return Temp;
}


bool	W25qxx_Init(void)
{
	u32	id;
	u8 i;

	for(i=0;i<10;i++)
	{
		id=W25qxx_ReadID();
		if((id & 0x00ffff00) == 0x00ef4000)break;
	}
	if(i>= 10)return 0;
	
	switch(id&0x0000FFFF)
	{
		case 0x401A:	// 	w25q512
			w25qxx.ID=W25Q512;
			w25qxx.BlockCount=1024;
		break;
		case 0x4019:	// 	w25q256
			w25qxx.ID=W25Q256;
			w25qxx.BlockCount=512;
		break;
		case 0x4018:	// 	w25q128
			w25qxx.ID=W25Q128;
			w25qxx.BlockCount=256;
		break;
		case 0x4017:	//	w25q64
			w25qxx.ID=W25Q64;
			w25qxx.BlockCount=128;
		break;
		case 0x4016:	//	w25q32
			w25qxx.ID=W25Q32;
			w25qxx.BlockCount=64;
		break;
		case 0x4015:	//	w25q16
			w25qxx.ID=W25Q16;
			w25qxx.BlockCount=32;
		break;
		case 0x4014:	//	w25q80
			w25qxx.ID=W25Q80;
			w25qxx.BlockCount=16;
		break;
		case 0x4013:	//	w25q40
			w25qxx.ID=W25Q40;
			w25qxx.BlockCount=8;
		break;
		case 0x4012:	//	w25q20
			w25qxx.ID=W25Q20;
			w25qxx.BlockCount=4;
		break;
		case 0x4011:	//	w25q10
			w25qxx.ID=W25Q10;
			w25qxx.BlockCount=2;
		break;
		default:
			return 0;
				
	}		
	w25qxx.PageSize=256;
	w25qxx.SectorSize=0x1000;
	w25qxx.SectorCount=w25qxx.BlockCount*16;
	w25qxx.PageCount=(w25qxx.SectorCount*w25qxx.SectorSize)/w25qxx.PageSize;
	w25qxx.BlockSize=w25qxx.SectorSize*16;
	w25qxx.CapacityInKiloByte=(w25qxx.SectorCount*w25qxx.SectorSize)/1024;
	W25qxx_ReadUniqID();
	W25qxx_ReadStatusRegister(1);
	W25qxx_ReadStatusRegister(2);
	W25qxx_ReadStatusRegister(3);
	return 1;
}


void W25qxx_ReadUniqID(void)
{
	u8 i;
	W25QXX_CS_CLR;
	W25qxx_Spi(0x4B);
	for(i=0;i<4;i++)
		W25qxx_Spi(W25QXX_DUMMY_BYTE);
	for(i=0;i<8;i++)
		w25qxx.UniqID[i] = W25qxx_Spi(W25QXX_DUMMY_BYTE);
	W25QXX_CS_SET;
}

void W25qxx_WaitBusy(void)
{
	W25qxx_Delay(1);
	W25QXX_CS_CLR;
	W25qxx_Spi(0x05);
	do
	{
		w25qxx.StatusRegister1 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		W25qxx_Delay(1);
	}
	while ((w25qxx.StatusRegister1 & 0x01) == 0x01);
	W25QXX_CS_SET;
}


u8 W25qxx_ReadStatusRegister(u8	SelectStatusRegister_1_2_3)
{
	u8	status=0;
	W25QXX_CS_CLR;
	if(SelectStatusRegister_1_2_3==1)
	{
		W25qxx_Spi(0x05);
		status=W25qxx_Spi(W25QXX_DUMMY_BYTE);	
		w25qxx.StatusRegister1 = status;
	}
	else if(SelectStatusRegister_1_2_3==2)
	{
		W25qxx_Spi(0x35);
		status=W25qxx_Spi(W25QXX_DUMMY_BYTE);	
		w25qxx.StatusRegister2 = status;
	}
	else
	{
		W25qxx_Spi(0x15);
		status=W25qxx_Spi(W25QXX_DUMMY_BYTE);	
		w25qxx.StatusRegister3 = status;
	}	
	W25QXX_CS_SET;
	return status;
}

#if 0

//###################################################################################################################

//###################################################################################################################
void W25qxx_WriteEnable(void)
{
	W25QXX_CS_CLR;
	W25qxx_Spi(0x06);
	W25QXX_CS_SET;
	W25qxx_Delay(1);
}
//###################################################################################################################
void W25qxx_WriteDisable(void)
{
	W25QXX_CS_CLR;
	W25qxx_Spi(0x04);
	W25QXX_CS_SET;
	W25qxx_Delay(1);
}
//###################################################################################################################

//###################################################################################################################
void W25qxx_WriteStatusRegister(u8 SelectStatusRegister_1_2_3,u8 dat)
{
	W25QXX_CS_CLR;
	if(SelectStatusRegister_1_2_3==1)
	{
		W25qxx_Spi(0x01);
		w25qxx.StatusRegister1 = dat;
	}
	else if(SelectStatusRegister_1_2_3==2)
	{
		W25qxx_Spi(0x31);
		w25qxx.StatusRegister2 = dat;
	}
	else
	{
		W25qxx_Spi(0x11);
		w25qxx.StatusRegister3 = dat;
	}
	W25qxx_Spi(dat);
	W25QXX_CS_SET;
}
//###################################################################################################################
void W25qxx_WaitForWriteEnd(void)
{
	W25qxx_Delay(1);
	W25QXX_CS_CLR;
	W25qxx_Spi(0x05);
	do
	{
		w25qxx.StatusRegister1 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		W25qxx_Delay(1);
	}
	while ((w25qxx.StatusRegister1 & 0x01) == 0x01);
	W25QXX_CS_SET;
}
//###################################################################################################################
	
//###################################################################################################################
void	W25qxx_EraseChip(void)
{
	W25qxx_WriteEnable();
	W25QXX_CS_CLR;
	W25qxx_Spi(0xC7);
	W25QXX_CS_SET;
	W25qxx_WaitForWriteEnd();
	W25qxx_Delay(10);
}
//###################################################################################################################
void W25qxx_EraseSector(u32 SectorAddr)
{
	W25qxx_WaitForWriteEnd();
	SectorAddr = SectorAddr * w25qxx.SectorSize;
	W25qxx_WriteEnable();
	W25QXX_CS_CLR;
	W25qxx_Spi(0x20);
	if(w25qxx.ID>=W25Q256)
		W25qxx_Spi((SectorAddr & 0xFF000000) >> 24);
	W25qxx_Spi((SectorAddr & 0xFF0000) >> 16);
	W25qxx_Spi((SectorAddr & 0xFF00) >> 8);
	W25qxx_Spi(SectorAddr & 0xFF);
	W25QXX_CS_SET;
	W25qxx_WaitForWriteEnd();
	W25qxx_Delay(1);
}
//###################################################################################################################
void W25qxx_EraseBlock(u32 BlockAddr)
{
	W25qxx_WaitForWriteEnd();
	BlockAddr = BlockAddr * w25qxx.SectorSize*16;
	W25qxx_WriteEnable();
	W25QXX_CS_CLR;
	W25qxx_Spi(0xD8);
	if(w25qxx.ID>=W25Q256)
		W25qxx_Spi((BlockAddr & 0xFF000000) >> 24);
	W25qxx_Spi((BlockAddr & 0xFF0000) >> 16);
	W25qxx_Spi((BlockAddr & 0xFF00) >> 8);
	W25qxx_Spi(BlockAddr & 0xFF);
	W25QXX_CS_SET;
	W25qxx_WaitForWriteEnd();
	W25qxx_Delay(1);
}
//###################################################################################################################
void W25qxx_WriteByte(u8 pBuffer, u32 WriteAddr_inBytes)
{
	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();
	W25QXX_CS_CLR;
	W25qxx_Spi(0x02);
	if(w25qxx.ID>=W25Q256)
		W25qxx_Spi((WriteAddr_inBytes & 0xFF000000) >> 24);
	W25qxx_Spi((WriteAddr_inBytes & 0xFF0000) >> 16);
	W25qxx_Spi((WriteAddr_inBytes & 0xFF00) >> 8);
	W25qxx_Spi(WriteAddr_inBytes & 0xFF);
	W25qxx_Spi(pBuffer);
	W25QXX_CS_SET;
	W25qxx_WaitForWriteEnd();
}
//###################################################################################################################
void W25qxx_WritePage	(u8 *pBuffer	,u32 Page_Address ,u32 NumByteToWrite_up_to_PageSize)
{
	if((NumByteToWrite_up_to_PageSize>w25qxx.PageSize)||(NumByteToWrite_up_to_PageSize==0))
		NumByteToWrite_up_to_PageSize=w25qxx.PageSize;
	W25qxx_WaitForWriteEnd();
  W25qxx_WriteEnable();
  W25QXX_CS_CLR;
  W25qxx_Spi(0x02);
	Page_Address = Page_Address*w25qxx.PageSize;
	if(w25qxx.ID>=W25Q256)
		W25qxx_Spi((Page_Address & 0xFF000000) >> 24);
  W25qxx_Spi((Page_Address & 0xFF0000) >> 16);
  W25qxx_Spi((Page_Address & 0xFF00) >> 8);
  W25qxx_Spi(Page_Address&0xFF);
	HAL_SPI_Transmit(&_W25QXX_SPI,pBuffer,NumByteToWrite_up_to_PageSize,100);	
	W25QXX_CS_SET;
  W25qxx_WaitForWriteEnd();
	W25qxx_Delay(1);
}
//###################################################################################################################
void  W25qxx_WriteSector(u8 *pBuffer	,u32 Sector_Address	,u32 NumByteToWrite_up_to_SectorSize)
{
	#if (_W25QXX_DEBUG==1)
	printf("w25qxx WriteSector %d begin...\r\n",Sector_Address);
	printf("w25qxx Split to %d Page...\r\n",w25qxx.SectorSize/w25qxx.PageSize);
	W25qxx_Delay(100);
	#endif	
	u8	inSectorIndex=0;
	if((NumByteToWrite_up_to_SectorSize>w25qxx.SectorSize)||(NumByteToWrite_up_to_SectorSize==0))
		NumByteToWrite_up_to_SectorSize=w25qxx.SectorSize;
	do
	{
		W25qxx_WritePage(pBuffer,(Sector_Address*w25qxx.SectorSize/w25qxx.PageSize)+inSectorIndex,NumByteToWrite_up_to_SectorSize);
		if(NumByteToWrite_up_to_SectorSize<=w25qxx.PageSize)
		{
			#if (_W25QXX_DEBUG==1)
			printf("w25qxx WriteSector done\r\n");
			W25qxx_Delay(100);
			#endif	
			return;
		}
		inSectorIndex++;
		NumByteToWrite_up_to_SectorSize-=w25qxx.PageSize;
		pBuffer+=w25qxx.PageSize;		
	}while(NumByteToWrite_up_to_SectorSize>0);
}
//###################################################################################################################
void 	W25qxx_WriteBlock	(u8* pBuffer ,u32 Block_Address		,u32	NumByteToWrite_up_to_BlockSize)
{
	u8	inBlockIndex=0;
	#if (_W25QXX_DEBUG==1)
	printf("w25qxx WriteSector %d begin...\r\n",Block_Address);
	printf("w25qxx Split to %d Page...\r\n",w25qxx.BlockSize/w25qxx.PageSize);
	W25qxx_Delay(100);
	#endif	
	if((NumByteToWrite_up_to_BlockSize>w25qxx.BlockSize)||(NumByteToWrite_up_to_BlockSize==0))
		NumByteToWrite_up_to_BlockSize=w25qxx.BlockSize;	
	do
	{
		W25qxx_WritePage(pBuffer,(Block_Address*w25qxx.BlockSize/w25qxx.PageSize)+inBlockIndex,NumByteToWrite_up_to_BlockSize);
		if(NumByteToWrite_up_to_BlockSize<=w25qxx.PageSize)
		{
			#if (_W25QXX_DEBUG==1)
			printf("w25qxx WriteSector done\r\n");
			W25qxx_Delay(100);
			#endif	
			return;
		}
		inBlockIndex++;
		NumByteToWrite_up_to_BlockSize-=w25qxx.PageSize;
		pBuffer+=w25qxx.PageSize;		
	}while(NumByteToWrite_up_to_BlockSize>0);	
}
//###################################################################################################################
void 	W25qxx_ReadByte		(u8 *pBuffer	,u32 Bytes_Address)
{
	#if (_W25QXX_DEBUG==1)
	u32	StartTime=HAL_GetTick();
	printf("w25qxx ReadByte at address %d begin...\r\n",Bytes_Address);
	#endif
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO,_W25QXX_CS_PIN,GPIO_PIN_RESET);
  W25qxx_Spi(0x0B);
	if(w25qxx.ID>=W25Q256)
		W25qxx_Spi((Bytes_Address & 0xFF000000) >> 24);
  W25qxx_Spi((Bytes_Address & 0xFF0000) >> 16);
  W25qxx_Spi((Bytes_Address& 0xFF00) >> 8);
  W25qxx_Spi(Bytes_Address & 0xFF);
	W25qxx_Spi(0);
	*pBuffer = W25qxx_Spi(W25QXX_DUMMY_BYTE);
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO,_W25QXX_CS_PIN,GPIO_PIN_SET);	
	#if (_W25QXX_DEBUG==1)
	printf("w25qxx ReadByte 0x%02X done after %d ms\r\n",*pBuffer,HAL_GetTick()-StartTime);
	#endif
}
//###################################################################################################################
void W25qxx_ReadBytes(u8* pBuffer, u32 ReadAddr, u32 NumByteToRead)
{
	#if (_W25QXX_DEBUG==1)
	u32	StartTime=HAL_GetTick();
	printf("w25qxx ReadBytes %d begin...\r\n",ReadAddr);
	#endif	
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO,_W25QXX_CS_PIN,GPIO_PIN_RESET);
	W25qxx_Spi(0x0B);
	if(w25qxx.ID>=W25Q256)
		W25qxx_Spi((ReadAddr & 0xFF000000) >> 24);
  W25qxx_Spi((ReadAddr & 0xFF0000) >> 16);
  W25qxx_Spi((ReadAddr& 0xFF00) >> 8);
  W25qxx_Spi(ReadAddr & 0xFF);
	W25qxx_Spi(0);
	HAL_SPI_Receive(&_W25QXX_SPI,pBuffer,ReadAddr,2000);	
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO,_W25QXX_CS_PIN,GPIO_PIN_SET);
	#if (_W25QXX_DEBUG==1)
	StartTime = HAL_GetTick()-StartTime; 
	for(u32 i=0;i<NumByteToRead ; i++)
	{
		if((i%8==0)&&(i>2))
		{
			printf("\r\n");
			W25qxx_Delay(10);
		}
		printf("0x%02X,",pBuffer[i]);		
	}
	printf("\r\n");
	printf("w25qxx ReadBytes done after %d ms\r\n",StartTime);
	W25qxx_Delay(100);
	#endif	
	W25qxx_Delay(1);
}

//###################################################################################################################
void 	W25qxx_ReadPage	  (u8 *pBuffer	,u32 Page_Address		,u32 NumByteToRead_up_to_PageSize)
{
	if((NumByteToRead_up_to_PageSize>w25qxx.PageSize)||(NumByteToRead_up_to_PageSize==0))
		NumByteToRead_up_to_PageSize=w25qxx.PageSize;
	#if (_W25QXX_DEBUG==1)
	printf("w25qxx ReadPage %d begin...\r\n",Page_Address);
	W25qxx_Delay(100);
	u32	StartTime=HAL_GetTick();
	#endif	
	Page_Address = Page_Address*w25qxx.PageSize;
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO,_W25QXX_CS_PIN,GPIO_PIN_RESET);
	W25qxx_Spi(0x0B);
	if(w25qxx.ID>=W25Q256)
		W25qxx_Spi((Page_Address & 0xFF000000) >> 24);
  W25qxx_Spi((Page_Address & 0xFF0000) >> 16);
  W25qxx_Spi((Page_Address& 0xFF00) >> 8);
  W25qxx_Spi(Page_Address & 0xFF);
	W25qxx_Spi(0);
	HAL_SPI_Receive(&_W25QXX_SPI,pBuffer,NumByteToRead_up_to_PageSize,100);	
	HAL_GPIO_WritePin(_W25QXX_CS_GPIO,_W25QXX_CS_PIN,GPIO_PIN_SET);
	#if (_W25QXX_DEBUG==1)
	StartTime = HAL_GetTick()-StartTime; 
	for(u32 i=0;i<NumByteToRead_up_to_PageSize ; i++)
	{
		if((i%8==0)&&(i>2))
		{
			printf("\r\n");
			W25qxx_Delay(10);
		}
		printf("0x%02X,",pBuffer[i]);		
	}	
	printf("\r\n");
	printf("w25qxx ReadPage done after %d ms\r\n",StartTime);
	W25qxx_Delay(100);
	#endif	
	W25qxx_Delay(1);
}
//###################################################################################################################
void 	W25qxx_ReadSector (u8 *pBuffer	,u32 Sector_Address	,u32 NumByteToRead_up_to_SectorSize)
{	
	u8	inSectorIndex=0;
	#if (_W25QXX_DEBUG==1)
	printf("w25qxx ReadSector %d begin...\r\n",Sector_Address);
	printf("w25qxx Split to %d Page...\r\n",w25qxx.SectorSize/w25qxx.PageSize);
	W25qxx_Delay(100);
	#endif	
	if((NumByteToRead_up_to_SectorSize>w25qxx.SectorSize)||(NumByteToRead_up_to_SectorSize==0))
		NumByteToRead_up_to_SectorSize=w25qxx.SectorSize;
	do
	{
		W25qxx_ReadPage(pBuffer,(Sector_Address*w25qxx.SectorSize/w25qxx.PageSize)+inSectorIndex,NumByteToRead_up_to_SectorSize);
		if(NumByteToRead_up_to_SectorSize<=w25qxx.PageSize)
		{
			#if (_W25QXX_DEBUG==1)
			printf("w25qxx ReadSector done\r\n");
			W25qxx_Delay(100);
			#endif	
			return;
		}
		inSectorIndex++;
		NumByteToRead_up_to_SectorSize-=w25qxx.PageSize;
		pBuffer+=w25qxx.PageSize;		
	}while(NumByteToRead_up_to_SectorSize>0);
}
//###################################################################################################################
void 	W25qxx_ReadBlock	(u8* pBuffer ,u32 Block_Address		,u32	NumByteToRead_up_to_BlockSize)
{
	u8	inBlockIndex=0;
	#if (_W25QXX_DEBUG==1)
	printf("w25qxx ReadSector %d begin...\r\n",Block_Address);
	printf("w25qxx Split to %d Page...\r\n",w25qxx.BlockSize/w25qxx.PageSize);
	W25qxx_Delay(100);
	#endif	
	if((NumByteToRead_up_to_BlockSize>w25qxx.BlockSize)||(NumByteToRead_up_to_BlockSize==0))
		NumByteToRead_up_to_BlockSize=w25qxx.BlockSize;	
	do
	{
		W25qxx_ReadPage(pBuffer,(Block_Address*w25qxx.BlockSize/w25qxx.PageSize)+inBlockIndex,NumByteToRead_up_to_BlockSize);
		if(NumByteToRead_up_to_BlockSize<=w25qxx.PageSize)
		{
			#if (_W25QXX_DEBUG==1)
			printf("w25qxx ReadSector done\r\n");
			W25qxx_Delay(100);
			#endif	
			return;
		}
		inBlockIndex++;
		NumByteToRead_up_to_BlockSize-=w25qxx.PageSize;
		pBuffer+=w25qxx.PageSize;		
	}while(NumByteToRead_up_to_BlockSize>0);	
}
//###################################################################################################################

#endif

