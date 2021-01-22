


#define FAT_OEM_NAME_LENGTH 8
#define FAT_LABEL_LENGTH 11
#define FAT_FS_TYPE_LENGTH 8
#define FAT_OS_BOOT_CODE_LENGTH 448

struct fat_preamble {
  u8 jump_instruction[3];
  u8 oem_name[FAT_OEM_NAME_LENGTH]; /* e.g. "mkdosfs\0" or "BSD  4.4" */
};

struct fat_bios_parameter_block {
  u16 bytes_per_sector; /* commonly 512 */
  u8 sectors_per_cluster; /* powers of 2, 1 ~ 128 */
  u16 reserved_sector_count; /* sectors before first FAT */
  u8 fat_count; /* almost always 2, sometimes 1, rarely more. */
  u16 max_root_entries;
  u16 total_sectors; /* if 0, check total_sectors_large */
  u8 media_descriptor; /* 0xF8 is fixed disk. First byte of each FAT should match this. */
  u16 sectors_per_fat;
  u16 sectors_per_track; /* for disks with geometry */
  u16 head_count; /* for disks with geometry, e.g. 2 for double-sided floppy. */
  u32 sectors_before_partition; /* zero for non-partitined disk. */
  u32 total_sectors_large; /* only used if total_sectors == 0, value > 65535 */
};

struct fat16_extended_bios_parameter_block {
  u8 physical_drive_number; /* 0x00 = removable, 0x80 = hard disk */
  u8 current_head; /* reserved, Windows NT uses bits one and two as flags. */
  u8 extended_boot_signature; /* 0x29 indicates the following three entries exist. */
  u32 serial_number;
  u8 label[FAT_LABEL_LENGTH]; /* volume label, padded with spaces (0x20) */
  u8 fs_type[FAT_FS_TYPE_LENGTH]; /* padded with spaces (0x20) e.g "FAT16   " */
};

struct fat16_boot_sector {
  struct fat_preamble preamble;
  struct fat_bios_parameter_block bios_params;
  struct fat16_extended_bios_parameter_block ext_bios_params;
};


void fat16_strncpy(u8* dst, u8* src, u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		*dst++ = *src++;
		if(*src == 0)break;
	}
}

void fat16_mkfs(u32 sector_num, u16 block_size, u16 sector_size)
{
	struct fat16_boot_sector fat16_bs;
	
	fat16_strncpy(fat16_bs.preamble.jump_instruction,"FAT",3);
	fat16_strncpy(fat16_bs.preamble.oem_name,"MSDOS5.0",8);

	//扇区大小
	fat16_bs.bios_params.bytes_per_sector = 64;
	//每个簇(block)包含的扇区数
	fat16_bs.bios_params.sectors_per_cluster = block_size/sector_size;
	fat16_bs.bios_params.reserved_sector_count = 64;
	//FAT表个数
	fat16_bs.bios_params.fat_count = 2;
	//根目录项数
	fat16_bs.bios_params.max_root_entries = ;
	//磁盘的扇区总数
	fat16_bs.bios_params.total_sectors = sector_num;

	fat16_bs.bios_params.media_descriptor = 0xF8;
	fat16_bs.bios_params.sectors_per_fat = sector_num/4/sector_size;
	//每个磁道包含的扇区数，不使用
	fat16_bs.bios_params.sectors_per_track = 0x00;
	//磁头数，不使用
	fat16_bs.bios_params.head_count = 0x00;

	//多个分区使用
	fat16_bs.bios_params.sectors_before_partition = 0x00;
	//在total_sectors为0时，使用该位置，用于大磁盘
	fat16_bs.bios_params.total_sectors_large = 0x00;
	
	fat16_bs.ext_bios_params.physical_drive_number = 0x80;
	fat16_bs.ext_bios_params.current_head = 0x01;
	fat16_bs.ext_bios_params.extended_boot_signature = 0x29;
	fat16_bs.ext_bios_params.serial_number = 0x12345678;
	fat16_strncpy(fat16_bs.ext_bios_params.label,"            ",FAT_LABEL_LENGTH);
	fat16_strncpy(fat16_bs.ext_bios_params.fs_type,"FAT16    ",FAT_FS_TYPE_LENGTH);

	fat16_sector_write();
	
}


void fat16_sector_write(u16 sector,u8* pdata, u16 len)
{

}

void fat16_sector_read(u16 sector,u8* pdata, u16 len)
{

}







#include <string.h>
#include<time.h>
#include "fat16.h"
#include "type.h"
#include "sddriver.h"

extern bool	WrtRedBLKPrt;
extern bool	SanDisk;

section("sdram0_bank1") WORD FAT[512*1024];
section("sdram0_bank1") BYTE RootDIR[32*1024];
section("sdram0_bank1") BYTE ImgNum[32*1024];
section("sdram0_bank1") BYTE ASCII2DEC[1024];

WORD 	BytePerSector;
WORD 	SectorPerCluster;
DWORD	BytePerCluster;

IDE_BUFFER buffers;

void fnSPI_FATChainInit()
{
//	printf("FAT Chain Init\n");
	DWORD	i;

//	for(i=0;i<512*1024;i++)
//		FAT[i]=0x0000;

	memset(FAT,0,1024*1024);
	
//	if(SanDisk)
		SD_ReadMultiBlockSanDisk(current_fs->fat_begin_lba,current_fs->bpb.sector_per_FAT,(BYTE *)FAT);
//	else
//		SD_ReadMultiBlock(current_fs->fat_begin_lba,current_fs->bpb.sector_per_FAT,(BYTE *)FAT);
}

void GetRootDIR()
{
//	printf("Root DIR Init\n");
	DWORD	i;

//	for(i=0;i<32*1024;i++)
//		RootDIR[i]=0x00;

	memset(RootDIR,0,32*1024);

//	if(SanDisk)
		SD_ReadMultiBlockSanDisk(current_fs->rootdir_begin_lba,current_fs->bpb.root_entrice*32/(current_fs->bpb.byte_per_sector),RootDIR);
//	else
//		SD_ReadMultiBlock(current_fs->rootdir_begin_lba,current_fs->bpb.root_entrice*32/(current_fs->bpb.byte_per_sector),RootDIR);
}

BYTE fnSPI_SectorByte(WORD sublocation)
{
	BYTE data=0;
	data=buffers.currentsector.u8data[sublocation];	  // Read Data from specified pos
	return data;
}

BYTE fnSPI_WriteSectorByte(WORD sublocation, BYTE value)
{
    buffers.currentsector.u8data[sublocation]=value;
	return value;
}

WORD fnSPI_SectorWord(WORD sublocation) // Return Word at position specified
{
	WORD data=0;

	data	=buffers.currentsector.u8data[sublocation];	       // Read Data from specified pos
	data   +=buffers.currentsector.u8data[sublocation+1]<<8;
	return data;					  			 // Return value
}

DWORD fnSPI_SectorDWORD(WORD sublocation) // Return Word at position specified
{
	DWORD data=0;

	data	=buffers.currentsector.u8data[sublocation];	  // Read Data from specified pos
	data   +=buffers.currentsector.u8data[sublocation+1]<<8;

	data   +=buffers.currentsector.u8data[sublocation+2]<<16;
	data   +=buffers.currentsector.u8data[sublocation+3]<<24;
	return data;					  			 // Return value
}

u8 fnFAT16_init(FS_t *fs,DWORD offset_sector)
{
	DWORD PartitionStart;

//	WrtRedBLKPrt=true;

	SD_ReadBlock(0,buffers.currentsector.u8data);

	if( buffers.currentsector.u8data[0]!=0xEB  || ( (buffers.currentsector.u8data[0]!=0xEB) && (buffers.currentsector.u8data[2]!=0x90)) )
	{
//		printf("Partition Start\n\n");
		PartitionStart=buffers.currentsector.u8data[0x01C9];
		PartitionStart=PartitionStart<<8 | buffers.currentsector.u8data[0x01C8];
		PartitionStart=PartitionStart<<8 | buffers.currentsector.u8data[0x01C7];
		PartitionStart=PartitionStart<<8 | buffers.currentsector.u8data[0x01C6];

		SD_ReadBlock(PartitionStart,buffers.currentsector.u8data);
	}

//	printf("WrtRedBLKPrt=%d\n",WrtRedBLKPrt);
//	WrtRedBLKPrt=false;

	// Load Parameters of FAT16
	fs->bpb.byte_per_sector 	=fnSPI_SectorWord(BYTE_PER_SECTOR);
	fs->bpb.sector_per_cluster 	=fnSPI_SectorByte(SECTOR_PER_CLUSTER);
	fs->bpb.reserve_sector 		=fnSPI_SectorWord(RESERVE_SECTOR);
	fs->bpb.FATnum 				=fnSPI_SectorByte(FAT_NUM);
	fs->bpb.root_entrice 		=fnSPI_SectorWord(ROOT_ENTRICE);
	fs->bpb.media_descriptor 	=fnSPI_SectorByte(MEDIA_DESCRIPTOR);
	fs->bpb.sector_per_track 	=fnSPI_SectorWord(SECTOR_PER_TRACK);
	fs->bpb.head_num			=fnSPI_SectorWord(HEAD_NUM);
	fs->bpb.hide_sector			=fnSPI_SectorDWORD(HIDE_SECTOR);
	fs->bpb.sector_num			=fnSPI_SectorDWORD(LARGE_SECTOR);
	fs->bpb.sector_per_FAT		=fnSPI_SectorWord(SECTOR_PER_FAT);

	fs->byte_per_cluster		=fs->bpb.byte_per_sector*fs->bpb.sector_per_cluster;
	fs->fat_begin_lba 			=offset_sector + fs->bpb.reserve_sector+fs->bpb.hide_sector;
	fs->rootdir_begin_lba 		=fs->fat_begin_lba + (fs->bpb.FATnum*fs->bpb.sector_per_FAT);
	fs->cluster_begin_lba 		=fs->rootdir_begin_lba+fs->bpb.root_entrice*32/fs->bpb.byte_per_sector;
	fs->fs_type					=FS_TYPE_FAT16;
	fs->total_cluster			=fs->bpb.sector_num/fs->bpb.sector_per_cluster;

//	#define DEBUG

	#ifdef DEBUG
		printf("fs->bpb.byte_per_sector=0x%x\n",fs->bpb.byte_per_sector);
		printf("fs->bpb.sector_per_cluster=0x%x\n",fs->bpb.sector_per_cluster);
		printf("fs->bpb.reserve_sector=0x%x\n",fs->bpb.reserve_sector);
		printf("fs->bpb.hide_sector=0x%x\n",fs->bpb.hide_sector);
		printf("fs->bpb.sector_num=0x%x\n", fs->bpb.sector_num);
		printf("fs->fat_begin_lba=0x%x\n", fs->fat_begin_lba);
		printf("fs->bpb.sector_per_FAT=0x%x\n", fs->bpb.sector_per_FAT);
		printf("fs->rootdir_begin_lba=0x%x\n", fs->rootdir_begin_lba);
		printf("fs->cluster_begin_lba=0x%x\n", fs->cluster_begin_lba);
		printf("fs->fs_type=0x%x\n", fs->fs_type);
		printf("fs->total_cluster=0x%x\n\n\n\n", fs->total_cluster);
	#endif

	BytePerSector 	=current_fs->bpb.byte_per_sector;
	SectorPerCluster=current_fs->bpb.sector_per_cluster;
	BytePerCluster	=BytePerSector*SectorPerCluster;

	fnSPI_FATChainInit();
	GetRootDIR();

	if (fnSPI_SectorWord(0x1FE)!=0xAA55) 				//check ending - 0xAA55 in DBR
		return 0x00;

	return 0x01;
}

/******************************************************************
//Function		: StringToUpper
//Description	: convert filename string to uppercase.
//Input			:
//				BYTE String - filename string
//return		: void
*******************************************************************/
void StringToUpper(BYTE *String)									//大小写转换，后缀名前的点
{
	BYTE	i,j;

	for(i=0;*(String+i) != '\0';i++)
	{
		if( (*(String+i)>=0x61) && (*(String+i)<=0x7A) )
			*(String+i) -=0x20;
	}

	for(i=0;*(String+i) != '\0';i++)
	{
		if( *(String+i)==0x2E )
		{
			for(j=i;*(String+j) != '\0';j++)
				*(String+j) =*(String+j+1);
			break;
		}
	}
//	printf("convert done!\n");
}

/******************************************************************
//Function		: SearchMaxImgNum
//Description	: find the filename in RootDIR.
//Input			: DWORD TotalNum
//return		: DWORD the max num of exist image file
*******************************************************************/
DWORD SearchMaxImgNum(DWORD TotalNum)
{
	DWORD	i,j,temp;

	if(TotalNum==0)
		return 0;
		
	for(i=0;i<TotalNum;i++)
		ASCII2DEC[i]=(*(ImgNum+i*5+0x00)-0x30)*10000+(*(ImgNum+i*5+0x01)-0x30)*1000+(*(ImgNum+i*5+0x02)-0x30)*100+(*(ImgNum+i*5+0x03)-0x30)*10+(*(ImgNum+i*5+0x04)-0x30);

	if(TotalNum==1)
		return ASCII2DEC[0];


	for (j=0;j<TotalNum-1;j++)			//冒泡法比较大小
		for (i=0;i<TotalNum-1-j;i++)
			if(ASCII2DEC[i]>ASCII2DEC[i+1])
			{
				temp	=ASCII2DEC[i];
				ASCII2DEC[i]=ASCII2DEC[i+1];
				ASCII2DEC[i+1]=temp;
			}

//	for(i=0;i<TotalNum;i++)
//	printf("Image Name:%d\n",ASCII2DEC[i]);

//	printf("Max Num=%d\n",ASCII2DEC[TotalNum-1]);

	return ASCII2DEC[TotalNum-1];
}

/******************************************************************
//Function		: FindFName
//Description	: find the filename in RootDIR.
//Input			: none
//return		: DWORD the max num of exist image file
*******************************************************************/
DWORD FindFName()
{
	DWORD	i=0,x=0;
	BYTE	LongOrd;
	BYTE 	fNameComp[12],fNameRoot[12];

	for(;(*(RootDIR+i*32)!=0x00 );)
	{
/*		if( *(RootDIR+i*32+0x0B)==0x0F )									//长文件名
		{
			LongOrd=*(RootDIR+i*32)-0x40;
			i+=LongOrd+1;
//			printf("Long file name\n");
		}
		else if((*(RootDIR+i*32)==0xE5))									//删除的文件名
		{
			i++;
//			printf("DIR had been deleted\n");
		}
//		else if( (*(RootDIR+i*32)==0x00 ) )									//目录为空
//		{
//			break;
//		}
		else */
		if( ( (*(RootDIR+i*32+0x00)==0x49) || (*(RootDIR+i*32+0x00)==0x69) )		//I & i
			 && ( (*(RootDIR+i*32+0x01)==0x4D) || (*(RootDIR+i*32+0x01)==0x6D) )		//M & m
			 && ( (*(RootDIR+i*32+0x02)==0x47) || (*(RootDIR+i*32+0x01)==0x67) )		//G & g
			 && ( (*(RootDIR+i*32+0x08)==0x42) || (*(RootDIR+i*32+0x08)==0x62) )		//B & b
			 && ( (*(RootDIR+i*32+0x09)==0x4D) || (*(RootDIR+i*32+0x09)==0x6D) )		//M & m
			 && ( (*(RootDIR+i*32+0x0A)==0x50) || (*(RootDIR+i*32+0x0A)==0x70) ) )		//P & p
		{
//			printf("copy dir\n");
			memcpy((void *)(ImgNum+x*5),(const void *)(RootDIR+i*32+0x03),5);
			x++;
			i++;
		}
		else
			i++;
	}

	return (SearchMaxImgNum(x));
}

/******************************************************************
//Function		: fnFAT16_NextEmptyCluster
//Description	: search next empty cluster in FAT buffer in SDRAM and fetch another 128KB data from HD FAT data table if not.
//Input			:
//				DWORD cluster - start cluster ( not used, routine search from cluster 2)
//Output		: Empty cluster number
*******************************************************************/
DWORD fnFAT16_NextEmptyCluster(DWORD cluster)
{
	DWORD	i;
	DWORD	SectorPerFAT=current_fs->bpb.sector_per_FAT;
	DWORD	BytePerSector=current_fs->bpb.byte_per_sector;

	
	for(i=cluster;i< ((SectorPerFAT*BytePerSector)/2);i++)
	{
		if( FAT[i]==0x0000 )		//find the empty cluster
			return i;
	}

	return 0xFFFF;
}

/******************************************************************
//Function		: fnFAT16_EditFATChain
//Description 	: Edit FAT table - only edit FAT buffer in SDRAM
//Input     	:
//            	DWORD Current_Cluster-target cluster number
//            	DWORD value-edit value
//Output   		: 1-done/ 0-fail
*******************************************************************/
bool fnFAT16_EditFATChain(DWORD Current_Cluster,DWORD value)
{
	if ( (Current_Cluster<2) || (Current_Cluster>=(current_fs->bpb.sector_per_FAT*current_fs->bpb.byte_per_sector/2)) )			//out of range
		return false;

	FAT[Current_Cluster]=value;
	return true;
}

/******************************************************************
//Function     	: fnFAT16_UpdateFAT
//Description  	: Update FAT data table in HD with fat buffer data in SDRAM
//				  write target sector( according to offset of target sector in cluster)
//Input     	:
//            	  DWORD - Current_Cluster - cluster in buffer
//return   		: 1-done/ 0-fail
*******************************************************************/
bool fnFAT16_UpdateFAT(DWORD Current_Cluster,DWORD ClusterNum)
{
	DWORD	FAT16_1UdStart=current_fs->fat_begin_lba;
	DWORD	FAt16_2UdStart=FAT16_1UdStart+current_fs->bpb.sector_per_FAT;
	BYTE	ret;

//	printf("Update FAT\n");

	ret=SD_WriteMultiBlock(FAT16_1UdStart,current_fs->bpb.sector_per_FAT,(BYTE *)FAT);

	if(ret!=0x00)
	{
//		printf("Update FAT1 error\n");
		return false;
	}
	ret=SD_WriteMultiBlock(FAt16_2UdStart,current_fs->bpb.sector_per_FAT,(BYTE *)FAT);

	if(ret!=0x00)
	{
//		printf("Update FAT2 error\n");
		return false;
	}
	return true;
}

/******************************************************************
//Function		: fnUpdateFILE
//Description 	: update file infor. including file name, file size, etc
//Input     	:
//				FILE_t *fp - file pointer
//Return   		: bool 1
*******************************************************************/
bool fnFAT16_UpdateFILE(FILE_t *fp)
{
	DWORD	i;
	BYTE	fwFileName[11];
	WORD	FstCluH,FstCluL;
	WORD	FileSizeH,FileSizeL;
	WORD	UdRootDIRSecNum;
	BYTE	ret;

//	printf("Update FILE\n");

	memcpy( (void *)fwFileName,(const void *)(fp->filename),11);
	StringToUpper(fwFileName);
	FstCluH=(WORD)(fp->FstClusLO>>16);
	FstCluL=(WORD)(fp->FstClusLO);
	FileSizeH=(WORD)(fp->FileSize>>16);
	FileSizeL=(WORD)(fp->FileSize);

	for(i=0;BytePerCluster/32;i++)			//update DIR in SDRAM
	{
		if(*(RootDIR+i*32)==0x00)			//this dir empty
		{
			memcpy((void *)(RootDIR+i*32),(const void *)fp,sizeof(FILE_t));
/*			memcpy( (void *)(RootDIR+i*32),(const void *)fwFileName,11);	//short file name
			*(RootDIR+i*32+11)=0x20;										//file attr
			*(RootDIR+i*32+12)=0x18;										//file NTres
			*(RootDIR+i*32+13)=0x00;										//file CrtTimeTeenth
			*(RootDIR+i*32+14)=0x00;										//file CrtTime
			*(RootDIR+i*32+15)=0x00;										//file CrtTime
			*(RootDIR+i*32+16)=0x00;										//file CrtDate
			*(RootDIR+i*32+17)=0x00;										//file CrtDate
			*(RootDIR+i*32+18)=0x00;										//file LastAccDate
			*(RootDIR+i*32+19)=0x00;										//file LastAccDate
			*(RootDIR+i*32+20)=(BYTE)FstCluH;								//file FstCluH
			*(RootDIR+i*32+21)=(BYTE)FstCluH>>8;							//file FstCluH
			*(RootDIR+i*32+22)=0x00;										//file WrtTime
			*(RootDIR+i*32+23)=0x00;										//file WrtTime
			*(RootDIR+i*32+24)=0x00;										//file WrtDate
			*(RootDIR+i*32+25)=0x00;										//file WrtDate
			*(RootDIR+i*32+26)=(BYTE)FstCluL;								//file FstCluL
			*(RootDIR+i*32+27)=(BYTE)FstCluL>>8;							//file FstCluL
			*(RootDIR+i*32+28)=(BYTE)FileSizeL;								//file FileSize
			*(RootDIR+i*32+29)=(BYTE)FileSizeL>>8;							//file FileSize
			*(RootDIR+i*32+30)=(BYTE)FileSizeH;								//file FileSize
			*(RootDIR+i*32+31)=(BYTE)FileSizeH>>8;							//file FileSize
*/
			break;
		}
	}

	UdRootDIRSecNum	=(i+1)*32/512+1;

	ret=SD_WriteMultiBlock(current_fs->rootdir_begin_lba,UdRootDIRSecNum,RootDIR);		//update DIR in SD Card

	if(ret!=0x00)
	{
		printf("Update Root DIR error\n");
		return false;
	}
	else
		return true;
}

/******************************************************************
//Function    	: fnWriteFile
//Description 	: write a data section from data buffer to target file
//Input       	:
//				BYTE *buffer-data buffer address
//             	DWORD size-data unit size
//             	DWORD count-number of data unit-data section size
//             	FILE_t *fp-file pointer
//Return      	: BOOL 1
*******************************************************************/
bool fnWriteFile(BYTE *buffer, DWORD size, DWORD count, FILE_t *fp)
{
	WORD	fwClusterNum	=(size*count+BytePerCluster-1)/BytePerCluster;
	WORD	fwSectorNum		=(size*count%(BytePerCluster)+BytePerSector-1)/BytePerSector;
	DWORD	CurrentSector	=(fp->FstClusLO-2)*current_fs->bpb.sector_per_cluster+current_fs->cluster_begin_lba;
	DWORD 	CurrentCluster	=(DWORD)( ( (DWORD)(fp->FstClusHI<<8) ) | (DWORD)fp->FstClusLO );
	DWORD	NextEmptyCluNum;
	BYTE	*fpwBMP;
	bool	fwDONE=false;
	BYTE	ret;

	fpwBMP	=buffer;

//	printf("fp->FstClusLO=%d CurrentCluster=%d\n",fp->FstClusLO,CurrentCluster);
//	printf("CurrentSector=%d\n",CurrentSector);

	while(1)
	{
		if(fwClusterNum!=1)									//完整的一簇数据
		{
//			WriteMultipleBlocks(CurrentSector,SectorPerCluster,fpwBMP);
			ret=SD_WriteMultiBlock(CurrentSector,SectorPerCluster,fpwBMP);

			if(ret==0x01)		/* 卡没完全插入卡中 card is not inserted entirely */
			{
//				printf("card is not inserted entirely,can't write image file,please insert SD Card!\n");
				return false;
			}
			else if(ret==0x00)
			{
				fpwBMP			+=BytePerCluster;
				CurrentSector	+=SectorPerCluster;
			}
			else
			{
				printf("write image file error\n");
				return false;
			}
//			printf("fwClusterNum=%d CurrentSector=0x%x\n",fwClusterNum,CurrentSector);
		}
		else												//剩下不到一簇的数据
		{
//			WriteMultipleBlocks(CurrentSector,fwSectorNum,fpwBMP);
			ret=SD_WriteMultiBlock(CurrentSector,fwSectorNum,fpwBMP);

			if(ret!=0x00)
			{
				printf("write last luster error\n");
				return false;
			}
			else
				fwDONE	=true;
				
//			printf("fwClusterNum=%d CurrentSector=0x%x\n",fwClusterNum,CurrentSector);
		}

		fwClusterNum--;

		if(fwClusterNum!=0)
		{
			NextEmptyCluNum	=fnFAT16_NextEmptyCluster(CurrentCluster);

			if(NextEmptyCluNum==0xFFFF)				//无可用的簇
			{
				printf("no availble cluster\n");
				return false;
			}

			ret=fnFAT16_EditFATChain(CurrentCluster,NextEmptyCluNum);
			if(ret==0x00)
			{
				printf("fnFAT16 EditFATChain error\n");
				return false;
			}

			ret=fnFAT16_EditFATChain(NextEmptyCluNum,0xFFFF);
			if(ret==0x00)
			{
				printf("fnFAT16 EditFATChain error\n");
				return false;
			}
			CurrentCluster	=NextEmptyCluNum;
		}

		if(fwDONE)
		{
			fwDONE	=false;
			fnFAT16_UpdateFAT(fp->FstClusLO,fwClusterNum);
			fnFAT16_UpdateFILE(fp);
			return true;
		}
	}
}

