

#include "bsp/config.h"
#include "bsp/bsp.h"

u8 crc8(u8* buf, int size)
{
	u8 crc = 0x00;
	u8 i;
	u8 poly = 0x1D;

	while (size--) {
		crc ^= *buf++;
		for (i = 0; i < 8; i++) {
			if (crc & 0x01) {
				crc = (crc >> 1) ^ poly;
			} else {
				crc >>= 1;
			}
		}
	}

	return crc;
}

