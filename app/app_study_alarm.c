
#include "bsp/config.h"

void task_ds3231(u8 task_id)
{
	os_addPeriod(task_id,1000000);
	ds3231_updateTime();
	
	ds3231_print_time();
	
	ds3231_getConv();
	
	os_debug(task_id);
	
}

//扫描按键
void task_scan_key(u8 task_id)
{

}

//创建所有的任务
void task_init()
{
	os_taskCreate(0,task_scan_key,10000);
    
}


