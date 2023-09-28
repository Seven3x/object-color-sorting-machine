#include "timer.h"
#include "led.h"
#include "main.h"
#include "motor.h"
#include "channel.h"
//////////////////////////////////////////////////////////////////////////////////	 

//STM32F407开发板
//定时器 驱动代码	   
							  
////////////////////////////////////////////////////////////////////////////////// 	 

uint8_t color_flag = 0;
uint32_t delay_count = 0;
uint16_t ucount = 0;;
uint8_t switch_lock = 0;
uint16_t switch_lock_count = 0;
uint8_t channel_pos[4] = {0, 1, 2, 3};
uint8_t last_color;
uint16_t color_count;
/**
 * @brief 延迟函数，单位ms，延迟必须为5的倍数，向下取整
 * 
 * @param n 
 */
void delay(uint32_t n) {
	delay_count = n / 5;
	while(delay_count);
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	static uint16_t interval_count;
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		int i = 0;
		// 四个门板开启时间计时
		static  door_count[4] = {0};

		//每隔 500ms 闪亮一次 确保单片机没有卡死
		if(++interval_count == 200 && !debug_flag) {  
			// LED2 = !LED2;
			interval_count = 0;
		}




		// 当碰到拨码开关时，执行该程序
		if(switch_state_once) {
			// 确保每次至少间隔1s再触发
			switch_lock_count = SWITCH_CHECK_TIME;
			switch_lock = 1;

			USART_SendString(USART1, "t\r\n");
			angle_count = (angle_count + 1) % 4;

			// 当 有物体 且 物体和桶号相同 时，打开门
			for(i = 0; i < 4; i++) {
				if (channel_item[i] == bucket[(7 - angle_count + i) % 4]) {
					//打开通道，会自动关闭
					channel_door[i] = 1;
					//物体清零
					channel_item[i] = 0;
				}
			}



			// 判断当前颜色
			if (current_color) {
				// 打开电机多少个 5ms 
				// motor_state_count = 200;
				ucount = COLOR_CHECK_TIME;
				motor_state = 1;
				// 放进东西
				channel_item[angle_count] = current_color;
				
				// 清空当前颜色
				current_color = 0;
				color_buffer[0] = color_buffer[1] = color_buffer[2] = 0;
				color_flag = 1;
			}


			

			// 清零该标志位
			switch_state_once = 0;
		}


		// 处理motor_state_count 	当motor_state_count为0时，关闭电机，标志位清0
		// if (motor_state_count == 0) {
		// 	motor_state = 0;
		// } else if (motor_state == 1){ //当motor_state为1时，motor_state_count自减
		// 	motor_state_count--;
		// }

		// 处理switch_lock
		if (switch_lock_count != 0) {
			switch_lock_count --;
		} else {
			switch_lock	= 0;
		}
		
		// 处理ucount 也即颜色检测间隔
		if (ucount != 0) {
			ucount --;
		}

		// 该if语句表示当颜色检测间隔结束时，清空串口缓冲区，继续接受串口信息
		if (ucount == 0 && color_flag == 1) {
			color_flag = 0;
			USART3_RX_STA = 0;
			current_color = 0;
			color_buffer[0] = color_buffer[1] = color_buffer[2] = 0;
		}
		
		// 处理color_count
		// 也即处理颜色检测到颜色后，传送带停止的延时
		if (current_color != 0 && last_color != current_color) {
			color_count = MOTOR_STOP_TIME;
		} else if (current_color != 0 && last_color == current_color) {
			if (color_count == 0) {
				motor_state = 0;
			} else {
				color_count -= 1;
			}
		} else {
			motor_state = 1;
		}
		last_color = current_color;

		// 处理串口延时
		if(color_buffer[0] == color_buffer[1] 
		&& color_buffer[1] == color_buffer[2]
		&& color_buffer[0] != 0
		&& ucount == 0) {
			current_color = color_buffer[0];
		} else if (ucount == 0){
			USART3_RX_STA = 0;
		}


		// 处理delay_count
		delay_count = delay_count > 0 ? delay_count - 1 : 0;
		
		// 处理color_count
		if (color_count >= 1) {
			color_count --;
		}

		// 防止门板开启时间过长
		for(i = 0; i < 4; i++) {
			// 当已有计时且门板关闭时，关闭计时  保险措施
			if(channel_door[i] == 0) {
				door_count[i] = 0;
			}

			// 当门板开启时，开始计时
			door_count[i] += channel_door[i];

			// 当计时超过 30*5ms 时，关闭门板
			if(door_count[i] >= CHANNEL_OPEN_TIME) {
				channel_door[i] = 0;
				door_count[i] = 0;
			}
		}


		// 根据motor_state的值控制电机
		Motor = motor_state;
		// 根据channel_door数组的值控制门板
		if(!debug_flag) {
			C0 = channel_door[0];
			C1 = channel_door[1];
			C2 = channel_door[2];
			C3 = channel_door[3];
		}
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

