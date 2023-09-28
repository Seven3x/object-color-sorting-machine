#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"

#define Motor PBout(5)
//MR 0 为正转，1 反转
#define MR PBout(1) 


/**
 * @brief 电机控制引脚初始化
 * 
 */
void Motor_Init(void);
void test_Motor(void);


#endif // !__M
