
#ifndef _TETRIS_H_
#define _TETRIS_H_

#define MAP_WIDTH                   10  // 地图宽,需要小于16
#define MAP_HEIGHT                  21  // 地图高


typedef struct
{
    s8 x;               //!< brick在地图中的x坐标
    s8 y;               //!< brick在地图中的y坐标
    s8 index;           //!< 方块索引, 高4位记录类型, 低4位记录变形
    u16 brick;         		//!< 方块数据,16个bit组成4*4的bitmap
} brick_t;

#define BRICK_TYPE                  7   // 一共7种类型的方块
#define BRICK_NUM_OF_TYPE           4   // 每一种类型有4种变形

#define BRICK_HEIGHT                4   // 一个brick由4*4的box组成
#define BRICK_WIDTH                 4

#define BRICK_START_X               ((MAP_WIDTH / 2) - (BRICK_WIDTH / 2))

#ifndef NULL
    #define NULL    ((void *)0)
#endif

/* Private macro -------------------------------------------------------------*/
// 对地图数组进行位操作的支持宏
#define     SET_BIT(dat, bit)      ((dat) |= (0x0001 << (bit)))
#define     CLR_BIT(dat, bit)      ((dat) &= ~(0x0001 << (bit)))
#define     GET_BIT(dat, bit)      (((dat) & (0x0001 << (bit))) >> (bit))

/* Private variables ---------------------------------------------------------*/
// 回调函数指针, 用来在坐标(x, y)画一个brick
static void (*draw_box)(u8 x, u8 y, u8 color) = NULL;
// 回调函数指针, 获取一个随机数
static u8 (*get_random_num)(void) = NULL;
// 回调函数指针, 当有消行时调用
static void (*return_remove_line_num)(u8 line) = NULL;

extern u16 map[MAP_HEIGHT];

// direction
typedef enum
{
    dire_left,      //!< 左移
    dire_right,     //!< 右移
    dire_down,      //!< 下移
    dire_rotate,    //!< 旋转
} dire_t;

void tetris_run(u8 key);
s8 tetris_init(void);

extern u8   tetris_move(dire_t direction);
extern void tetris_sync(void);
extern void tetris_sync_all(void);
extern u8   tetris_is_game_over(void);
void create_new_brick(brick_t* brick);
void draw_brick(const brick_t* brick);
void clear_brick ( const brick_t* brick );

// 初始化, 需要的回调函数说明:
// 在(x, y)画一个box, color为颜色, 注意0表示清除, 不表示任何颜色
// draw_box_to_map(u8 x, u8 y, u8 color)

// 函数须返回一个随机数, 产生新方块使用
// get_random(void)

// 当产生新方块后回调此函数, 参数为新方块的数据
// 当前版本 *info 为uint16_t型数据, 代表新方块的点阵数据
// next_brick_info(const void *info)

// 当发生消行时回调此函数, 参数为消除的行数
// remove_line_num(u8 line)
extern void tetris_start(
    void (*draw_box_to_map)(u8 x, u8 y, u8 color),
    u8 (*get_random)(void),
    void (*remove_line_num)(u8 line)
    );

#endif

