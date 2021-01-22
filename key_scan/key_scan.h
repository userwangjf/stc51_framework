

#ifndef __KEY_SCAN_H__
#define __KEY_SCAN_H__


#define KEY_UP		0xf6
#define KEY_DOWN	0xee
#define KEY_LEFT	0xf5
#define KEY_RIGHT	0xf3
#define KEY_START	0xdb
#define KEY_Y		0xed
#define KEY_X		0xeb
#define KEY_A		0xdd
#define KEY_B		0xde

#define KEY_VALID(k)	(k!=0xff)


//消抖动，按键扫描次数。如果连续5次都是扫描的都是相同键值，则认为是有效键值，否则是误触发
#define KEYDEBOUNCE 		5
//重复触发间隔
#define KEY_REPEAT_DLY	 	5
//重复触发等待
#define KEY_REPEAT_WAIT		20

//声明变量
extern u8 g_uiCurrKey;            //当前按键值
extern u8 g_uiLastKey;            //上次按键值
extern u8 g_uiKeyScanCount;       //按键扫描计数，作用：消抖动

extern u8 g_uiKeyDown;            //键被按下，返回的键值。       作用：单次按键，单次返回有效键值；按住不放，也只返回被按下的一个键值
extern u8 g_uiKeyRelease;         //键被释放后，返回的键值。     作用：只有按下的按键被释放后，才返回按下的键值
extern u8 g_uiKeyContinue;        //键连续按键，重复返回的键值。 作用：只要按住不放，就会重复地返回相同键值

//函数声明
void key_scan ( void );
u8 key_get();

#endif

