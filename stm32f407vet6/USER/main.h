#ifndef __MAIN_H
#define __MAIN_H
#define USART3_REC_LEN  			3  	//定义最大接收字节数 200
#include "sys.h"
extern u8 USART3_RX_BUF; 
extern uint8_t angle_count;
extern uint8_t channel_item[4];
extern uint8_t bucket[4];
extern uint8_t item;
extern uint8_t channel_door[4];
extern u16 USART3_RX_STA;

#endif // !__
