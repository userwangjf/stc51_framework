

#ifndef __APP_H__
#define __APP_H__

//被main调用
void task_init();

//根据不同的应用，使用不同的头文件,只能包含1个

#include "app/app_os_test.h"

//#include "app/app_study_alarm.h"


#endif

