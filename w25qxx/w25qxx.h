#ifndef _W25QXX_H
#define _W25QXX_H
	 
typedef enum
{
	W25Q10=1,
	W25Q20,
	W25Q40,
	W25Q80,
	W25Q16,
	W25Q32,
	W25Q64,
	W25Q128,
	W25Q256,
	W25Q512,
	
}W25QXX_ID_t;

typedef struct
{
	W25QXX_ID_t	ID;
	u8			UniqID[8];

	u16  PageSize;
	u32	PageCount;
	u32	SectorSize;
	u32	SectorCount;
	u32	BlockSize;
	u32	BlockCount;

	u32	CapacityInKiloByte;
	
	u8 StatusRegister1;
	u8 StatusRegister2;
	u8 StatusRegister3;
	
}w25qxx_t;

extern w25qxx_t	w25qxx;
//############################################################################
// in Page,Sector and block read/write functions, can put 0 to read maximum bytes 
//############################################################################

u8 W25qxx_Spi(u8 dat);
u32 W25qxx_ReadID(void);
void W25qxx_WaitBusy(void);
bool	W25qxx_Init(void);

void W25qxx_ReadUniqID(void);
u8 W25qxx_ReadStatusRegister(u8	SelectStatusRegister_1_2_3);

void	W25qxx_EraseChip(void);
void W25qxx_EraseSector(u32 SectorAddr);
void W25qxx_EraseBlock(u32 BlockAddr);

void W25qxx_WriteByte(u8 pBuffer,u32 Bytes_Address);
void W25qxx_WritePage(u8 *pBuffer,u32 Page_Address,u32 NumByteToWrite_up_to_PageSize);
void W25qxx_WriteSector(u8 *pBuffer,u32 Sector_Address,u32 NumByteToWrite_up_to_SectorSize);
void W25qxx_WriteBlock(u8* pBuffer,u32 Block_Address,u32 NumByteToWrite_up_to_BlockSize);

void W25qxx_ReadByte(u8 *pBuffer,u32 Bytes_Address);
void W25qxx_ReadBytes(u8 *pBuffer,u32 ReadAddr,u32 NumByteToRead);
void W25qxx_ReadPage(u8 *pBuffer,u32 Page_Address,u32 NumByteToRead_up_to_PageSize);
void W25qxx_ReadSector(u8 *pBuffer,u32 Sector_Address,u32 NumByteToRead_up_to_SectorSize);
void W25qxx_ReadBlock(u8* pBuffer,u32 Block_Address,u32 NumByteToRead_up_to_BlockSize);




#endif

