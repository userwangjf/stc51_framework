#include "type.h"

//FAT32 PARTITION TABLE
#define PARTITION_TABLE_1		(0x1BE)
#define PARTITION_TABLE_2		(0x1CE)
#define PARTITION_TABLE_3		(0x1DE)
#define PARTITION_TABLE_4		(0x1EE)
#define PARTITION_TABLE_SIZE	(16)

#define ACTICE_FLAG		(0)
#define START_HEAD		(1)
#define START_SECTOR	(2)
#define START_CYLINDER	(2)
#define SYSTEM_ID		(4)
#define END_HEAD		(5)
#define END_SECTOR		(6)
#define END_CYLINDER	(6)
#define OFFSET_SECTOR	(8)
#define TOTAL_SECTOR	(12)

#define SIGN_POSITION	(0x1FE)
#define SIGN_VALUE		(0xAA55)

//FAT32 File system type
#define FAT32_TYPECODE1		(0x01)
#define FAT32_TYPECODE2		(0x0B)
#define FAT32_TYPECODE3		(0x0C)
#define FAT16_TYPECODE1		(0x04)
#define FAT16_TYPECODE2		(0x06)
#define FAT16_TYPECODE3		(0x0E)

#define EXT_TYPECODE1		(0x05)
#define EXT_TYPECODE2		(0x0F)

#define FS_TYPE_FAT12		(0x00)
#define FS_TYPE_FAT16		(0x01)
#define FS_TYPE_FAT32		(0x02)

#define NO_PARTITION		(0x00)

#define FILE_LABEL_SHORTNAME		(0x00)
#define FILE_LABEL_RESERVE1			(0x0C)
#define FILE_LABEL_CREATETIME10ms	(0x0D)
#define FILE_LABEL_CREATETIME		(0x0E)
#define FILE_LABEL_CREATEDATE		(0x10)
#define FILE_LABEL_LASTVISITDATE	(0x12)
#define FILE_LABEL_CLUSTERHIGH		(0x14)
#define	FILE_LABEL_LASTEDITTIME		(0x16)
#define FILE_LABEL_LASTEDITDATE		(0x18)
#define FILE_LABEL_CLUSTERLOW		(0x1A)
#define FILE_LABEL_FILESIZE			(0x1C)

//FAT32 File Attributes and Types
#define FILE_ATTR_READ_ONLY   	0x01
#define FILE_ATTR_HIDDEN 		0x02
#define FILE_ATTR_SYSTEM 		0x04
#define FILE_ATTR_SYSHID		0x06
#define FILE_ATTR_VOLUME_ID 	0x08
#define FILE_ATTR_DIRECTORY		0x10
#define FILE_ATTR_ARCHIVE  		0x20
#define FILE_ATTR_LFN_TEXT		0x0F
#define FILE_HEADER_BLANK		0x00
#define FILE_HEADER_DELETED		0xE5

//Boot Parameter Block
#define BYTE_PER_SECTOR		(0x0B)
#define SECTOR_PER_CLUSTER	(0x0D)
#define RESERVE_SECTOR		(0x0E)
#define FAT_NUM				(0x10)
#define	ROOT_ENTRICE		(0x11)
#define SMALL_SECTOR		(0x13)
#define MEDIA_DESCRIPTOR	(0x15)
#define SECTOR_PER_FAT		(0x16)
#define SECTOR_PER_TRACK	(0x18)
#define HEAD_NUM			(0x1A)
#define HIDE_SECTOR			(0x1C)
#define LARGE_SECTOR		(0x20)
#define EXT_BOOT_SIGN		(0x26)
#define	VOLUME_SERIAL_NUM	(0x27)
#define VOLUME_LABLE		(0x2B)

//Boot Parameter Block
typedef struct{

	WORD	byte_per_sector;
	BYTE	sector_per_cluster;
	WORD	reserve_sector;
	BYTE	FATnum;
	WORD	root_entrice;
	BYTE	media_descriptor;
	WORD	sector_per_track;
	WORD	head_num;
	DWORD	hide_sector;
	DWORD	sector_num;
	DWORD	sector_per_FAT;
	WORD	flag;
	WORD	version;
	DWORD	root_cluster;
	WORD	FSinfo_sector;
	WORD	backup_sector;
	BYTE	res2[12];
	
	BYTE	bios_drive;
	BYTE	res3;
	BYTE	ext_boot_sign;
	DWORD	serial_num;
	char	label[11];
	char	file_system[8];
	
}BPB_FAT16_t;

//extern BPB_FAT16_t BPB_FAT16;

typedef struct
{
	BPB_FAT16_t bpb;
	BYTE 		fs_type;
	DWORD		byte_per_cluster;
	DWORD 		fat_begin_lba;
	DWORD 		cluster_begin_lba;
	DWORD 		rootdir_begin_lba;
	DWORD		total_cluster;
}FS_t;

extern FS_t *current_fs;

typedef void (*fnFS_init_t)(FS_t *,DWORD);

typedef struct
{
	WORD	flag;
	DWORD 	offset_sector;
	DWORD 	total_sector;
	FS_t  	*fs;
	fnFS_init_t 	fnFS_init;
}partition_table_t;

extern partition_table_t *partition_table[23];

extern void fnPartition_init(BYTE* ,BYTE);
extern void fnFAT32_init(FS_t *fs,DWORD offset_sector);

typedef struct
{
	DWORD value;
	DWORD offset;
}currentcluster_t;

currentcluster_t fnGetCurrentCluster(DWORD curp,DWORD startcluster);

typedef struct
{
	BYTE	filename[11];
	BYTE	Attr;
	BYTE	NTRes;
	BYTE	CrtTimeTeenth;
	WORD	CrtTime;
	WORD	CrtDate;
	WORD	LastAccDate;
	WORD	FstClusHI;
	WORD	WrtTime;
	WORD	WrtDate;
	WORD	FstClusLO;
	DWORD	FileSize;
}FILE_t;

typedef struct
{
	BYTE short_name[11];
	BYTE type;
	BYTE reserve1;
	BYTE create_time_10ms;
	WORD create_time;
	WORD create_date;
	WORD last_visit_date;
	WORD cluster_high;
	WORD last_edit_time;
	WORD last_edit_date;
	WORD cluster_low;
	DWORD filesize;
}file_label_t;

typedef struct
{
	BYTE num;
	WORD name1[5];
	BYTE flag;
	BYTE reserve;
	BYTE chksum;
	WORD name2[6];
	WORD startcluster;
	WORD name3[2];
}lfn_label_t;

typedef struct
{
	FILE_t** list;
	DWORD    num;
}FILELIST_t;

typedef struct
{  
	DWORD	startcluster;		
	DWORD	curp;
	BYTE	filename[12];	
	DWORD	filesize;	 
}FILE_s;

void fnDetect_Partition(void);
DWORD fnDetectDirectory(DWORD startcluster);
FILE_t *fnCreateFile(DWORD startcluster,BYTE *name);

#define __tolower(x) ((x<='Z'&&x>='A')?(x+0x20):x)
#define __toupper(x) ((x<='z'&&x>='a')?(x-0x20):x)


#ifndef SEEK_SET
#define 	SEEK_SET (0)
#define 	SEEK_CUR (1)
#define 	SEEK_END (2)
#endif

#define    IOFBF			0x00
#define    IOLBF			0x01
#define		IONBF			0x02
#define  	MODE_BUF		0x03
#define 	MODE_FREEBUF	0x04
#define 	MODE_FREEFILE	0x08
#define 	MODE_READ		0x10
#define 	MODE_WRITE		0x20
#define 	MODE_RDWR		0x30
#define 	MODE_READING	0x40
#define 	MODE_WRITING	0x80
#define 	MODE_EOF		0x100
#define		MODE_ERR		0x200
#define 	MODE_UNGOT		0x400
#define		MODE_OPEN		0x800

#define		O_APPEND	(0x01)
#define 	O_CREAT		(0x02)
#define		O_EXCL		(0x04)
#define 	O_RDONLY	(0x08)
#define 	O_RDWR		(0x10)
#define		O_TRUNC		(0x20)
#define		O_WRONLY	(0x40)

#define 	FIND_FILE_MODE 	(0)
#define		FIND_DIR_MODE  	(1)
#define 	FIND_ALL_MODE	(2)

#define EOF		(-1)
#define NULL	(0)

FILELIST_t* fnListDirectory(DWORD startcluster);
void  fnListDir(DWORD startcluster);
FILE_t *fnFindFile(DWORD startcluster,BYTE *filename,BYTE mode);
FILE_t *fnOpenFile(FILE_t* fp,BYTE mode,BYTE isOpen);
BYTE fnUpdateFILE(FILE_t *fp, bool Finished);
DWORD fnReadFile(BYTE *buffer, WORD size, DWORD count, FILE_t *fp);

//------------------------------------------------------------------
typedef union 
{
	unsigned char 	u8data[512];
	unsigned short 	u16data[256];
	unsigned long 	u32data[128];
}BUFFER;

typedef struct
{
 	BUFFER currentsector;
	DWORD SectorCurrentlyLoaded; 
} IDE_BUFFER;

extern IDE_BUFFER buffers;

//WORD	LastClusterNum;
