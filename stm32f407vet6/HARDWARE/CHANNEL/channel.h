#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "sys.h"

#define C0 PDout(1)
#define C1 PDout(3)
#define C2 PDout(5)
#define C3 PDout(7)

/**
 * @brief 通道门控制引脚初始化
 * 
 */
void Channel_Init(void);

#endif // !__
