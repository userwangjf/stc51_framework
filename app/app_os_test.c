
#include "bsp/config.h"

u32 cnt_task1 = -1;
u32 cnt_task2 = -1;
u32 cnt_task3 = -1;
u32 cnt_task4 = -1;

void os_task1(u8 task_id) {

	//这2句只能用一个
    //os_addDelay(task_id,200000);
	os_addPeriod(task_id,200000);
	
    cnt_task1++;
	printk("\r\ntask1:%d",(u32)cnt_task1);
	led8_595_num(cnt_task1,cnt_task2,cnt_task3,cnt_task4);

	delay_ms(1);    

	#if OS_DBG_EN
	os_debug(task_id);
	#endif
}


void os_task2(u8 task_id) {
    //os_addDelay(task_id,400000);
	os_addPeriod(task_id,400000);
	
    cnt_task2++;
	printk("\r\ntask2:%d",(u32)cnt_task2);
	led8_595_num(cnt_task1,cnt_task2,cnt_task3,cnt_task4);

	#if OS_DBG_EN
	os_debug(task_id);
	#endif
}

void os_task3(u8 task_id) {
    //os_addDelay(task_id,800000);
	os_addPeriod(task_id,800000);
	
    cnt_task3++;
	printk("\r\ntask3:%d",(u32)cnt_task3);
	led8_595_num(cnt_task1,cnt_task2,cnt_task3,cnt_task4);
    
	#if OS_DBG_EN
	os_debug(task_id);
	#endif
}


void os_task4(u8 task_id) {
    //os_addDelay(task_id,1600000);
	os_addPeriod(task_id,1600000);
	
    cnt_task4++;
	printk("\r\ntask4:%d %d %d %d",(u32)cnt_task1,(u32)cnt_task2,(u32)cnt_task3,(u32)cnt_task4);
	led8_595_num(cnt_task1,cnt_task2,cnt_task3,cnt_task4);
    
	#if OS_DBG_EN
	os_debug(task_id);
	#endif
}


void task_init()
{
	//不同的启动延迟，将任务错峰
    os_taskCreate(0,os_task1,10000);
    os_taskCreate(1,os_task2,20000);
    os_taskCreate(2,os_task3,30000);
    os_taskCreate(3,os_task4,40000);
}

