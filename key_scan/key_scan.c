
#include "bsp/config.h"
#include "app/key_scan.h"

//======================================================
//KeyScan.c
//======================================================
//注意：该宏定义，定义在keyscan.h文件中
//#define KEYDEBOUNCE 0x05             //消抖动，按键扫描次数。如果连续5次都是扫描的都是相同键值，则认为是有效键值，否则是误触发

u8 		g_uiCurrKey;            //当前按键值
u8 		g_uiLastKey = 0xff;            //上次按键值
u8 		g_uiKeyScanCount;       //按键扫描计数，作用：消抖动

u8 		g_uiKeyDown = 0xff;            //键被按下，返回的键值。       作用：单次按键，单次返回有效键值；按住不放，也只返回被按下的一个键值
u8 		g_uiKeyRelease;         //键被释放后，返回的键值。     作用：只有按下的按键被释放后，才返回按下的键值
u8 		g_uiKeyContinue;        //键连续按键，重复返回的键值。 作用：只要按住不放，就会重复地返回相同键值
u8		g_repeatDly;

u8		g_repeatWait = 0;

//每10ms调用一次该函数，用作按键扫描
//P3口的低6位作为按键
//没有按键时，返回的是0xff，
void key_scan ( void )
{
	static u8 LastReadKey;        //上次从IO口读取的键值 ,注意是静态变量
	u8 	CurrReadKey;               //当前从IO口读取的键值

//---------------------- 需要修改的地方 --------------------------------------//
	u8  KeyRow,KeyColumn;

	//读取按键的行数
	P3 &= 0xf8;
	P3 |= 0x38;
	KeyRow = P3 | 0xc0;

	//读取按键的列数
	P3 &= 0xc7;
	P3 |= 0x07;
	KeyColumn = P3 | 0xc0;

	//读取行列数，就是当前键值了
	CurrReadKey = KeyRow|KeyColumn;         //获取当前的键值

	//如果当前读取的键值与上次从IO口读取的键值相同
	if ( CurrReadKey == LastReadKey )
	{
		//首先判断是否大于等于debounce的设定值(即是，是否大于等于设定的采样次数)
		if ( g_uiKeyScanCount >= KEYDEBOUNCE )
		{
			//按住不放,多次响应
			g_uiCurrKey = CurrReadKey;                //如果是,将当前的读取值判定为有效按键值(如果是，在采样周期中，都是这个值，则判定为有效按键值)

			//按住不放只响应一次
			if ( g_uiLastKey == g_uiCurrKey )
			{
				//重复触发前等待
				if(g_repeatWait++ >= KEY_REPEAT_WAIT)
				{
					//重复触发周期
					if ( g_repeatDly++ >= KEY_REPEAT_DLY )
					{
						g_uiKeyDown = g_uiCurrKey;
						g_repeatDly = 0;
					}
					else
					{
						g_uiKeyDown = 0xff;    //没有键值
					}
				}
			}
			else
			{
				g_uiKeyDown = g_uiCurrKey;             //如果不同，按键有效,(就是第一次有效值时)
				g_repeatDly = 0;
				g_repeatWait = 0;
			}

			//按键释放时，按键值才有效
			if ( g_uiCurrKey == 0xff )               //当有效按键值从非0到0的状态时(即是，从有按键到无按键，表示已经释放了)，表示之前按键已经释放了
			{
				g_uiKeyRelease = g_uiLastKey;
			}

			g_uiLastKey = g_uiCurrKey;               //记录上次有效按键值
		}
		else                                       //如果否，则debounce加一(如果否，则继续采样键值)
		{
			g_uiKeyScanCount++;
		}
	}
	else                                          //如果当前读取的键值与上次从IO口读取的键值不同，说明按键已经变化
	{
		g_uiKeyDown = 0xff;                       //放开按键后第一次进入扫描程序，清零g_uiKeyDown.作用：消除一个BUG（你猜BUG是什么？）
		g_uiKeyScanCount = 0;                     //清零之前的按键的debounce计数
		LastReadKey = CurrReadKey;                //将当前读取的键值记录为上次读取的按键值
		g_repeatDly = 0;
		g_repeatWait = 0;
	}
}

u8 key_get()
{
	u8 tmp;
	if ( g_uiKeyDown != 0xff )
	{
		tmp = g_uiKeyDown;
		g_uiKeyDown = 0xff;
		return tmp;
	}
	return 0xff;
}

