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
uint8_t angle_count;

/**
 * 每个通道当前是否存放物体
 */
uint8_t channel_item[4];

/**
 * 通道门状态
 * 通过TIM6不断扫描该数组，如果为1则打开通道门，为0则关闭通道门
 */
uint8_t channel_door[4];

/**
 * 四个桶的编号，范围 0~3，可以修改此处以修改桶内存放的物体，分别和物体的0~3对应
 */
uint8_t bucket[4] = {0, 1, 2, 3};




int main(void)
{ 
	u8 t;
	u8 len;	
	u16 times=0;  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2

	delay_init(168);		//延时初始化 
	uart_init(115200);	//串口初始化波特率为115200
	KEY_Init();
	LED_Init();		  		//初始化与LED连接的硬件接口  
	Channel_Init();			//门板初始化
	Motor_Init();			//电机初始化
	TIM3_Int_Init(50-1,8400-1);	//分频系数8400 计数50次，84000000/8400/50=200Hz=5ms
	Openmv_Init();			//openmv初始化
	while(1)
	{
		// //根据按键控制门板
		// if(KEY_Scan(0)==1) {
		// 	C1 = 1;
		// }
		// else {
		// 	C1 = 0;
		// }
		if((USART3_RX_STA & 0x8000) !=0)//接收完成
		{
			USART_SendData(USART1, USART3_RX_BUF);         //向串口1发送数据
		    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			USART3_RX_STA = 0;
		}
		// if(KEY_Scan(1) == 1) {
		// 	angle_count += 0;
		// }





		// USART_SendData(USART1, USART_RX_BUF[t]);         //向串口1发送数据
		// while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			
	}
}

