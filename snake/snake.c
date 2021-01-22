

#include "bsp/config.h"

#define SNAKE_WIDTH		11
#define SNAKE_HEIGHT	21
#define SNAKE_MAX		128

enum direc{up, down, left, right};//蛇的运动方向

typedef struct
{
	s8 x[SNAKE_MAX];	//snake.x[0]为蛇头
	s8 y[SNAKE_MAX];
	u8 length;			//蛇的节数
	u8 direc;			/*蛇移动方向*/
	int life_time;		//蛇存活时间
	u8 life;			//蛇的生命,1活着,0死亡
}snake_body;

snake_body snake;

typedef struct
{
    u8 x;//食物的横坐标
    u8 y;//食物的纵坐标
} snake_food;

snake_food next_food;
u8 snake_speed = 50;
u8 snake_scores = 0;


void snake_next_food()
{
	u8 i;
	//获得随机数种子;

_restart:
	next_food.x = random() % SNAKE_WIDTH;
	next_food.y = random() % SNAKE_HEIGHT;
		
	for(i=0;i<(snake.length);i++)//检测产生的食物是否与蛇本身重叠
	{
		if((next_food.x==snake.x[i])&&(next_food.y==snake.y[i]))
		{
			goto _restart;
		}
	}
}

//蛇爬行算法
void snake_run()	
{	
	u8 i;
	for(i=(snake.length-1);i>0;i--)		//蛇头作为前进引导，长江后浪推前浪 
	{
		snake.y[i]=snake.y[i-1];
		snake.x[i]=snake.x[i-1];
	}
	switch(snake.direc)	//根据蛇的方向标前进
	{
		case up: 	snake.y[0]-=1;	break;
		case down:	snake.y[0]+=1;	break;
		case left:	snake.x[0]-=1;	break;
		case right:	snake.x[0]+=1;	break;
	}
}

//检测是否吃到食物
void snake_eat()
{
	if((snake.x[0]==next_food.x) && (snake.y[0]==next_food.y))//是否吃到食物
	{
		if(snake.length < SNAKE_MAX)
			snake.length++;

		snake_next_food();
	}
}

void snake_die()
{
	u8 i;

	if((snake.x[0]>SNAKE_WIDTH-1)||(snake.x[0]<0)||(snake.y[0]>SNAKE_MAX-1)||(snake.y[0]<0))//是否碰壁
	{
		snake.life=0;	//蛇碰到墙壁死亡
	}
	for(i=3;i<snake.length;i++)//从蛇的第四节开始判断是否撞到自己了，因为蛇头不可能撞到二三四节
	{
		if((snake.x[i]==snake.x[0])&&(snake.y[i]==snake.y[0]))//是否自残
			snake.life=0;	//蛇头碰到自身死亡
	}
}


void snake_disp()	//游戏画面显示
{
	u8 i;

	disp_clear_row(0,20);
	for(i=0;i<snake.length;i++)
	{	
		disp_on(snake.x[i],snake.y[i]);
	}

	disp_on(next_food.x,next_food.y);
}

u8 snake_key(u8 key)
{
	//按下UP键时，只有蛇在水平方向时才能改变
	//按下DOWN键时，只有蛇在水平方向时才能改变
	//按下right键时，只有蛇在垂直方向时才能改变
	if(key == KEY_UP)
	{
		if((snake.direc==left)||(snake.direc==right))
		{
			snake.direc=up;
			//return key;
		}
	}
	else if(key == KEY_DOWN)
	{
		if((snake.direc==left)||(snake.direc==right))
		{
			snake.direc=down;
			//return key;
		}
	}
	else if(key == KEY_LEFT)
	{
		if((snake.direc==up)||(snake.direc==down))
		{
			snake.direc=left;
			//return key;
		}
	}
	else if(key == KEY_RIGHT)
	{
		if((snake.direc==up)||(snake.direc==down))
		{
			snake.direc=right;
			//return key;
		}
	}
	return key;
}

void snake_init()
{	
	u8 c;	
	for(c=0;c<SNAKE_MAX;c++)//清除内存数据
	{
		snake.x[c]=-1;
		snake.y[c]=-1;
	}
	snake.life = 1;			//赋予蛇生命
	snake.length = 3;		//默认长度3节
	snake.direc = up;		//初始方向
	snake.x[0]=5;
	snake.y[0]=15;
	snake.x[1]=5;
	snake.y[1]=16;
	snake.x[2]=5;
	snake.y[2]=17;
	snake_scores = 0;
	next_food.x = 5;
	next_food.y = 8;
}

u8 snake_step = 0;
//10ms调用一次该函数
void snake_game(u8 key)
{
	snake_step++;

	snake_die();
	
	if((0xff != snake_key(key)) || (snake_step >= snake_speed))
	{
		snake_step=0;
		snake_run();
		snake_eat();
		snake_disp(); 	
	}
	
}




