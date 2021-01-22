
#ifndef __XMODEM_H__
#define __XMODEM_H__

#define XMODEM_MAX_RETRY 	60
#define XMODEM_BUF_LEN 		132		//使用校验和方式

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

struct xmodem_receiver {
	s16 (*get_char)(void); 	 	//接收字符函数，带超时
	void (*put_char)(u8 c);		//发送字符
	u8 (*get_first)(void);		//接收第一个字符。
	void (*delay_1s)(void); 	//延时1秒。
	u8 (*writer)(u8* buff, u8 size);	//接收一帧数据后保存
	u8* rx_buf;							//临时接收缓存，需要大于132.
	u32 rx_ok_cnt;						//接收成功的帧计数
};

s8 xmodem_rx(struct xmodem_receiver *rx);

void xmodem_init(u8* rx_buf);

#endif

