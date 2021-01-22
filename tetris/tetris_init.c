
#include "bsp/config.h"
#include "music/music.h"

//定义使用的按键
#define JK_PAUSE	KEY_Y
#define JK_ROTATE	KEY_A
#define JK_DOWN		KEY_DOWN
#define JK_LEFT		KEY_LEFT
#define JK_RIGHT	KEY_RIGHT
#define JK_RESET		KEY_START


static u8   pause = 0;  		// 游戏暂停
static u8   tetris_level = 1;          // 级别
static u16  lines = 0;          // 消除的行数
static u16  tetris_score = 0;          // 分数
static u16  time_count = 0;


//更新打印
void game_info_update()
{
	disp_dec(0,0,(tetris_score/100)%10);
	disp_dec(4,0,(tetris_score/10)%10);
	disp_dec(8,0,tetris_score%10);
	//L-level
	disp_row(6,0x00);
	disp_row(7,0x00);
	disp_row(8,0x00);
	disp_row(9,0x00);
	disp_row(10,0x00);
	disp_row(11,0x00);
	disp_on(0,6);
	disp_on(0,7);
	disp_on(0,8);
	disp_on(0,9);
	disp_on(0,10);
	disp_on(1,10);
	disp_on(2,10);
	disp_on(4,8);
	disp_on(5,8);
	disp_on(6,8);
	disp_dec(8,6,tetris_level);
}

void tetris_pause ( void )
{
	pause = !pause;

	if ( pause )
		game_info_update();
	else
	{
		tetris_sync_all();    // 因为打印暂停破坏了地图区显示
	}
	// 所以退出时要刷新整个地图区
	return;
}

//回调函数，计算分数
void tetris_get_score ( u8 line )
{
	u16 i;
	u16 j;
	lines += line;

	switch ( line )
	{
		case 1:
			tetris_score += 1;
			music_on(1);
			break;
		case 2:
			tetris_score += 3;
			music_on(1);
			break;
		case 3:
			tetris_score += 5;
			music_on(1);
			break;
		case 4:
			tetris_score += 12;
			music_on(1);
			break;
		default:
			break;
	}

	

	// 每25行升一级
	tetris_level = lines / 25;

	if(line > 0)
	{
		for(i=0;i<100*line;i++)
		{
			//P54 = ~P54;
			for(j=0;j<1500;j++);
		}
	}

	return;
}

//回调函数，用于在屏幕上画出box
//color为0时清除此坐标上的box
void tetris_draw_box ( u8 x, u8 y, u8 color )
{
	if ( color == 0 )
	{
		disp_off ( x,y );
	}
	else
	{
		disp_on ( x,y );
	}

	return;
}

//回调函数，用于生成随机数
u8 tetris_random()
{
	return random();
}

//返回下一个方块，用于预览显示
void tetris_preview_brick ( const void* info )
{
	info = info;
	return;
}



brick_t brick;
//定时调用此函数
void tetris_run ( u8 key )
{
	static u8 refresh = 0;

	if(key == JK_RESET)
	{
		tetris_init();
		tetris_score = 0;
		tetris_level = 0;
		return;
	}
	
	//游戏结束
	if ( tetris_is_game_over() )
	{
		game_info_update();
		return;
	}

	if ( !pause )
	{
		time_count++;
	}
	else
		refresh = 0;

	// 自由向下移动，级别越高速度越快
	if ( time_count >= ( 100 - tetris_level*10 ) )
	{
		time_count = 0;

		refresh = 1;
		tetris_move ( dire_down );
	}

	//有按键按下
	if ( KEY_VALID ( key ) )
	{
		// 暂停时只响应回车键
		if ( pause && key != JK_PAUSE )
		{
			return;
		}

		switch ( key )
		{
			case JK_ROTATE:
				tetris_move ( dire_rotate );
				break;
			case JK_DOWN:
				tetris_move ( dire_down );
				break;
			case JK_LEFT:
				tetris_move ( dire_left );
				break;
			case JK_RIGHT:
				tetris_move ( dire_right );
				break;
			case JK_PAUSE:
				tetris_pause();
				return;
				break;
			default:
				break;
		}

		refresh = 1;
	}

	if ( refresh )
	{
		refresh = !refresh;
		//tetris_sync();
		tetris_sync_all();
		// 更新行数, 分数等信息
		//game_info_update();
	}

	return;
}


s8 tetris_init ( void )
{

	// 当发生消行时回调此函数, 参数为消除的行数
	// remove_line_num(uint8_t line)
	tetris_start ( tetris_draw_box, tetris_random, tetris_get_score );

	tetris_pause();

	return 0;
}