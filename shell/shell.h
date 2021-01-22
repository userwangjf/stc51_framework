
#ifndef __SHELL_H__
#define __SHELL_H__

#include "bsp/config.h"

#define SHELL_CMD_MAX_LEN		140
#define SHELL_ECHO_ON			1

#define SHELL_ECHO(dat)		Uart1_Echo(dat)
#define SHELL_TX(dat)		Uart1_Tx(dat)

extern u8 hex_str[11];

//void printk(char *str,...);
void shell_rx_dat(u8 dat);
void shell_init(void);
void shell_run(void);
u8* hex2str(u32 hex);
u8* byte2str(u8 hex);
u8 str2dec(u8 *str);
u8 *dec2str(u32 dec);
u32 str2hex(u8 *str);

#endif

