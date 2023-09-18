#include "key.h"
#include "delay.h" 
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	 

// STM32F407开发板
//按键输入驱动代码	   
							  
////////////////////////////////////////////////////////////////////////////////// 	 



//按键初始化函数
void KEY_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA,时钟
 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//WK_UP对应引脚PA0--K1按键
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;// PA2对应开关
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA1
 
} 
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 --对应K1按键
//key 0,2 对应PA0 PA2
u8 KEY_Scan(u8 mode, u8 key)
{	
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(PAin(key)==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(PAin(key)==1)return 1;
	}else if((PAin(key))==0)key_up=1; 	    
 	return 0;// 无按键按下
}

// 拨码开关扫描函数
// 该函数修改switch_state_once和switch_state的值
// key 0/2 表示 PA0 PA2
// value 0-1 表示读取的是低电平还是高电平
void DIP_Scan(u8 key, u8 value) {
	static u8 key_up=1;//按键按松开标志
	u8 key_value = 0;

	if (PAin(key) == value) {
		delay_ms(10);//去抖动 


		if (PAin(key) == value) {
			
			switch_state = 1;

			if (key_up) {
				key_up = 0;
				switch_state_once = 1;
			}
		} else {
			switch_state = 0;
			key_up  = 0;
		}
		
	} else {
		key_up = 1;
		switch_state = 0;
	}
}



void test_DIP(void) {
	debug_flag = 1;
	while(1) {
		DIP_Scan(2, 0);
		LED2 = switch_state;
	}
}

















