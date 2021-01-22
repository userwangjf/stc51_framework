
#include "bsp/config.h"
#include "bsp/bsp.h"

//使用计数器0、1来产生随机数
u8 random()
{
	u8 temp[2];

	temp[0] = TL0 + TL1;
	temp[1] = TH0 + TH1;
	
	return crc8(temp,2);
}

