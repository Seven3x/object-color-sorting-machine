#include "delay.h"
#include "usart.h"
#include "led.h"
#include "exti.h"
#include "timer.h"
#include "key.h"
#include "channel.h"
#include "motor.h"
#include "main.h"
#include "openmv.h"
#include "lcd1602.h"

/**
 * 传送带上物品的颜色
 * 四种颜色，分别用 0~3 表示
 * 0:红
 * 1:黄
 * 2:蓝
 * 3:绿
 */
uint8_t item;

/**
 * 表示每个通道旋转了几个90°，最大值为3，初始值为0
 */
uint8_t angle_count=0;

/**
 * 每个通道当前是否存放物体
 * 0: 无方块
 * 1: 红
 * 2: 黄
 * 3: 蓝
 * 4: 绿
 */
uint8_t channel_item[4];

/**
 * 通道门状态
 * 通过TIM6不断扫描该数组，如果为1则打开通道门，为0则关闭通道门
 */
uint8_t channel_door[4];

/**
 * 四个桶的编号，范围 1~4
 * 可以修改此处以修改桶内存放的物体，分别和物体的0~3对应
 */
uint8_t bucket[4] = {1, 2, 4, 8};

/**
 * 拨码开关状态量，持续按下视为一次
 * 0: 未按下
 * 1: 按下
 */
uint8_t switch_state_once = 0;
/**
 * 拨码开关状态量，持续按下视为多次
 * 0: 未按下
 * 1: 按下
 */
uint8_t switch_state = 0;

/**
 * 传送带电机开启时间
 * 单位为 5ms
 */
uint16_t motor_state_count = 0;

/**
 * 传送带传送允许标志位
 * 0: 不允许传送
 * 1: 允许传送
 * 该标志为1时，如果传送带电机开启时间不为0，则传送带电机开启
 */
uint8_t motor_state = 0;

/**
 * 当前方块颜色
 * 0: 无方块
 * 1: 红
 * 2: 黄
 * 3: 蓝
 * 4: 绿
 */
uint8_t current_color = 0;

uint8_t message[] = "c0:0 c1:0 c2:0 c3:0\r\n";
uint8_t message_angle[] = " A:0\r\n";
uint8_t angle_count_last;
uint8_t msg_flag;
uint8_t debug_flag = 0;
uint8_t msg_lock = 0;

int main(void)
{ 
	u8 t;
	u8 len;	
	u16 times=0;  
	u8 i = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2

	delay_init(168);		//延时初始化 
	uart_init(115200);	//串口初始化波特率为115200
	KEY_Init();
	LED_Init();		  		//初始化与LED连接的硬件接口  
	Channel_Init();			//门板初始化
	Motor_Init();			//电机初始化
	TIM3_Int_Init(50-1,8400-1);	//分频系数8400 计数50次，84000000/8400/50=200Hz=5ms
	Openmv_Init();			//openmv初始化

	// delay_ms(1000);

	// LCD_Init();
	delay_ms(100);

	// LCD_ShowStr(2,0,message); 
	motor_state = 1;
	// delay_ms(1000);
	while(1)
	{

		//***********测试电磁铁********
		// test_channel();
		
		//***********测试电机***********
		// test_Motor();

		//***********测试串口***********
		// test_usart();

		//***********测试拨码开关***********
		// test_DIP();

		//***********测试通道号对应*****
		if(channel_item[0] != 0 
		|| channel_item[1] != 0 
		|| channel_item[2] != 0
		|| channel_item[3] != 0){
			message[3] 	= 	'0' + channel_item[0];
			message[8] 	= 	'0' + channel_item[1];
			message[13]	= 	'0' + channel_item[2];
			message[18]	= 	'0' + channel_item[3];
			if (msg_flag && !msg_lock) {
				USART_SendString(USART1, message);
			}
		}
		msg_lock = msg_flag;
		msg_flag= channel_item[0] || channel_item[1] || channel_item[2] || channel_item[3];

		message_angle[3] =   '0' + angle_count;
		if (angle_count_last != angle_count) {
			USART_SendString(USART1, message_angle);
		
		} 
		angle_count_last = angle_count;

		// if ((USART_RX_STA&0x8000)!=0) {
		// 	channel_item[0] = USART_RX_BUF[0];
		// 	channel_item[1] = USART_RX_BUF[1];
		// 	channel_item[2] = USART_RX_BUF[2];
		// 	channel_item[3] = USART_RX_BUF[3];
		// }
		

		// 每个循环检测一次拨码开关，值返回到switch_state_once和switch_state
		// LED2 = KEY_Scan(1, 0);
		DIP_Scan(2, 0);
		// LED2 = switch_state_once;
		LED2 = !!current_color;
		
		// delay_ms(1000);
		
			
	}
}

