#include "channel.h"
#include "usart.h"
#include "main.h"

/**
 * @brief 通道门控制引脚初始化
 * 
 */
void Channel_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    // 初始化引脚 D1、D3、D5、D7 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    // 置零
    C0 = 0;
    C1 = 0;
    C2 = 0;
    C3 = 0;
}

void test_channel(void) {
    debug_flag = 1;
    delay_ms(200);
    C0 = 1;
    delay_ms(200);
    C1 = 1;
    C0 = 0;
    delay_ms(200);
    C2 = 1;
    C1 = 0;
    delay_ms(200);
    C3 = 1;
    C2 = 0;
    delay_ms(200);
    C3 = 0;
    delay_ms(3000);
}