

#include "bsp/config.h"
#include "shell/shell.h"

extern s16 pos_x ;
extern s16 pos_y ;

bool shell_rx_ok;
u8 shell_rx_ptr = 0;
//用于共享的缓存
u8 shell_rx_buf[SHELL_CMD_MAX_LEN];
u8 shell_tx_ptr = 0;

//周期性的调用此函数，处理shell命令
void shell_run(void)
{

	if(!shell_rx_ok)return;

	if(shell_rx_buf[0] == 'X' && shell_rx_buf[1] == 'M')
	{
		SHELL_TX("\r\nStart XModem:\r\n");
		delay_ms(200);
		xmodem_init(shell_rx_buf);
	}
	else if(shell_rx_buf[0] == 'R' && shell_rx_buf[1] == 'D')
	{													  	
	}
	else
		SHELL_TX("\r\nOK\r\n");

	shell_rx_ok = 0;

}

void shell_init(void)
{
	shell_rx_ptr = 0;
}


//接收字符，需要在uart中断函数中调用。
void shell_rx_dat(u8 dat)
{
	if(dat == '\n' || dat == '\r')
	{
		if(shell_rx_ptr > 0)
			shell_rx_ok = 1;

		shell_rx_buf[shell_rx_ptr] = 0;
		shell_rx_ptr = 0;
	}
	else if(shell_rx_ptr < SHELL_CMD_MAX_LEN)
	{
		shell_rx_buf[shell_rx_ptr++] = dat;
		shell_tx_ptr++;
	}
	else
	{
		shell_rx_ptr = 0;
		shell_tx_ptr = 0;
		shell_rx_ok = 1;
	}
	
	#if SHELL_ECHO_ON
		SHELL_ECHO(dat);
	#endif
}


//将16进制的
//最大支持的字符串长度为10B。
//格式:0xaaab3cd0
u32 str2hex(u8 *str) 
{
	u32 temp=0;
	u8 i;
	if(*str++ != '0')return 0;
	if(*str++ != 'x')return 0;
	for(i=10;i>0;i--)
	{
		if(*str == 0)break;
		temp = temp * 16;
		if(*str >= '0' && *str <= '9')
			temp += (*str - '0');
		else if(*str >= 'a' && *str <= 'f')
			temp += (*str - 'a' + 10);
		else if(*str >= 'A' && *str <= 'F')
			temp += (*str - 'A' + 10);  
		else
			return 0;

		str++;
	}
	return temp;
}

u8 hex_str[11];
//转换为16进制的字符串
u8* hex2str(u32 hex)
{
	u8 i;
	u8 tmp;

	hex_str[10] = 0;
	if(hex == 0)
	{
		hex_str[9] = '0';
		i=8;
	}
	else
	{
		for(i=9;i>1;i--)
		{
			if(hex == 0)break;
			tmp = hex & 0xf;
			if(tmp > 9)
				hex_str[i] = 'a' + tmp - 10;
			else
				hex_str[i] = '0' + tmp;
	
			hex >>= 4;
		}
	}
	hex_str[i--] = 'x';
	hex_str[i] = '0';

	return &hex_str[i];		
}

//
u8* byte2str(u8 hex)
{
	static u8 str[3];
	u8 tmp;

	str[0] = '0';
	str[1] = '0';
	str[2] = 0;

	tmp = hex & 0x0f;
	if(tmp > 9)
		str[1] = 'a' + tmp - 10;
	else
		str[1] = '0' + tmp;

	hex >>= 4;
	tmp = hex & 0x0f;
	if(tmp > 9)
		str[0] = 'a' + tmp - 10;
	else
		str[0] = '0' + tmp;
	
	return str;		
}

//转换长度为3的字符串为10进制数。
u8 str2dec(u8 *str)
{
	u8 temp;
	u8 i;

	temp = 0;

	for(i=0;i<3;i++)
	{
		if(str[i] >= '0' && str[i] <= '9')
		{
		 	temp *= 10;
			temp += (str[i] - '0');
		}
		else
			return temp;	
	}
	return temp;
}

u8 *dec2str(u32 dec)
{
	static u8 dstr[11];
	s8 i;
	u8 num;

	dstr[10] = 0;

	for(i=9;i>=0;i--)
	{
		num = dec % 10;
		dstr[i] = num + '0';
		dec -= num;
		if(dec == 0)return &dstr[i];
		dec /= 10;
	}
	return &dstr[i];
}

