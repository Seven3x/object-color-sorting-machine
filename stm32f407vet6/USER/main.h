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
extern uint32_t delay_count;
extern uint8_t debug_flag;
extern uint8_t switch_state;
extern uint16_t motor_state_count;
extern uint8_t motor_state;
extern uint8_t switch_state_once;
extern uint8_t current_color;
extern u8 bcount;
extern u8 color_buffer[4];
extern uint8_t switch_lock;
extern uint16_t switch_lock_count;

#define clip(x) (((x) > 3) ? (x) - 4 : (x))

#endif // !__
