

//包含需要的模块驱动
#include "led8_595/led8_595.c"
#include "ds3231/ds3231.c"


void module_init()
{
	//点亮数码管
	led8_595_test();

}




