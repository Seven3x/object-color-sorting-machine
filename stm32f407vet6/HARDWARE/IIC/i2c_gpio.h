#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

#include "sys.h"
#include <inttypes.h>

#define ADDRESS		0x7e


#define LCD1602_I2C_WR	0		/* 写控制bit */
#define LCD1602_I2C_RD	1		/* 读控制bit */


/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define LCD1602_GPIO_PORT_I2C	GPIOE			/* GPIO端口 */
#define LCD1602_RCC_I2C_PORT 	RCC_AHB1Periph_GPIOE		/* GPIO端口时钟 */
#define LCD1602_I2C_SCL_PIN		GPIO_Pin_11			/* 连接到SCL时钟线的GPIO */
#define LCD1602_I2C_SDA_PIN		GPIO_Pin_13			/* 连接到SDA数据线的GPIO */


/* 定义读写SCL和SDA的宏，已增加代码的可移植性和可阅读性 */
#if 1	/* 条件编译： 1 选择GPIO的库函数实现IO读写 */
	#define LCD1602_I2C_SCL_1()  GPIO_SetBits(LCD1602_GPIO_PORT_I2C, LCD1602_I2C_SCL_PIN)		/* SCL = 1 */
	#define LCD1602_I2C_SCL_0()  GPIO_ResetBits(LCD1602_GPIO_PORT_I2C, LCD1602_I2C_SCL_PIN)		/* SCL = 0 */
	
	#define LCD1602_I2C_SDA_1()  GPIO_SetBits(LCD1602_GPIO_PORT_I2C, LCD1602_I2C_SDA_PIN)		/* SDA = 1 */
	#define LCD1602_I2C_SDA_0()  GPIO_ResetBits(LCD1602_GPIO_PORT_I2C, LCD1602_I2C_SDA_PIN)		/* SDA = 0 */
	
	#define LCD1602_I2C_SDA_READ()  GPIO_ReadInputDataBit(LCD1602_GPIO_PORT_I2C, LCD1602_I2C_SDA_PIN)	/* 读SDA口线状态 */
#else	/* 这个分支选择直接寄存器操作实现IO读写 */
    #define LCD1602_I2C_SCL_1() (PBout(9) = 1)		/* SCL = 1 */
	#define LCD1602_I2C_SCL_0() (PBout(9) = 0)		/* SCL = 0 */
	#define LCD1602_I2C_SDA_1() (PBout(8) = 1)		/* SDA = 1 */
	#define LCD1602_I2C_SDA_0() (PBout(8) = 0)		/* SDA = 0 */
	#define LCD1602_I2C_SDA_READ() (PBin(8))			/* 读SDA口线状态 */
#endif


void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);
uint8_t  i2c_WriteByte(uint8_t data);
uint8_t i2c_R_Byte(void);
void i2c_CfgGpio(void);

	

#endif

