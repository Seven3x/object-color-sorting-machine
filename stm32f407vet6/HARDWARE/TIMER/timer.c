#include "timer.h"
#include "led.h"
#include "main.h"
#include "channel.h"
//////////////////////////////////////////////////////////////////////////////////	 

//STM32F407开发板
//定时器 驱动代码	   
							  
////////////////////////////////////////////////////////////////////////////////// 	 


//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	static uint16_t interval_count;
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		int i = 0;
		// 四个门板开启时间计时
		int door_count[4] = {0};

		if(++interval_count == 100) {  //每隔 500ms 闪亮一次
			LED2 = !LED2;
			interval_count = 0;
		}
		
		for(i = 0; i < 4; i++) {
			// 当 有物体 且 通道号和桶号相同 时，打开门
			channel_door[i] = channel_item[i] && (i == bucket[clip(i + angle_count)]);
			
			
			// // 判断通道内是否有物体
			// if (channel_item[i] == 1) {
			// 	// 判断桶号和通道号是否相同（也即是否同种物体）
			// 	if(i == bucket[(i + angle_count) > 3 ? (i + angle_count - 4) : (i + angle_count)]) {
			// 		//打开通道
			// 		channel_door[i] = 1;
			// 	}
			// }
			
		}


		// 防止门板开启时间过长
		for(i = 0; i < 4; i++) {
			//if(door_count[i] == 0 &&);
		}

		// 根据channel_door数组的值控制门板
		C0 = channel_door[0];
		C1 = channel_door[1];
		C2 = channel_door[2];
		C3 = channel_door[3];

	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}




//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

