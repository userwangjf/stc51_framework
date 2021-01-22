/******************************************************************************************************
** Descriptions:		sd 卡驱动软件包: SD卡相关工具函数 ---- CRC校验头文件
********************************************************************************************************/

#ifndef 	__sdcrc_h
#define 	__sdcrc_h

#include "sdconfig.h"
#include "sddriver.h"
#include "sdhal.h"
#include "sdcmd.h"

extern INT16U SD_GetCRC16(INT8U *pSource,INT16U len);	//得到len个字节的CRC16

extern INT8U SD_GetCmdByte6(INT8U cmd,INT8U *param);	//得到SD命令的第6个字节: CRC7 + 停止位1

extern INT8U SD_GetCRC7(INT8U *pSource,INT16U len);		//得到len个字节的CRC7

#endif
