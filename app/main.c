

#include "bsp/config.h"

void main()
{

	//初始化stc单片机内部
	bsp_init();

	//外部模块初始化
	module_init();
		
	//初始化操作系统
	os_init();
	
	//任务初始化
	task_init();

	//启动操作系统
	os_start();

	while(1);
	
}




