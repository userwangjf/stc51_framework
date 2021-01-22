
#include "bsp/config.h"
#include "music/music.h"

u32 mucis_ptr_play = 0;
u8 music_play_on = 0;
u32 music_ptr_end = 6268;


u8 music_play()
{
	u8 tmp;
	if(music_play_on)
	{
		if(mucis_ptr_play >= music_ptr_end)
		{
			mucis_ptr_play = 0;
			music_play_on = 0;
		}
		tmp = music_dat1[mucis_ptr_play++];
		return tmp;
		/*
		if(tmp > 0x80)
			return 0x80 + (tmp - 0x80) /2;
		else
			return 0x80 - (0x80 - tmp) / 2;*/
	}
	
	return 0x80;
}

void music_on(u8 on)
{
	music_play_on = on;	
}


