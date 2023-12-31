/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   用gpio模拟i2c总线, 适用于STM32系列CPU。该模块不包括应用层命令帧，仅包括I2C总线基本操作函数。
  ******************************************************************************

  */ 


#include "i2c_gpio.h"
#include "sys.h"
#include "led.h"

//static void i2c_CfgGpio(void);



/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到的。
    工作条件：CPU主频72MHz ，MDK编译环境，1级优化
  
		循环次数为10时，SCL频率 = 205KHz 
		循环次数为7时，SCL频率 = 347KHz， SCL高电平时间1.5us，SCL低电平时间2.87us 
	 	循环次数为5时，SCL频率 = 421KHz， SCL高电平时间1.25us，SCL低电平时间2.375us 
	*/
	for (i = 0; i < 11; i++);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	LCD1602_I2C_SDA_1();
	LCD1602_I2C_SCL_1();
	i2c_Delay();
	LCD1602_I2C_SDA_0();   //拉低
	i2c_Delay();
	LCD1602_I2C_SCL_0();   //释放SCL线
	i2c_Delay();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	LCD1602_I2C_SDA_0();
	LCD1602_I2C_SCL_1();
	i2c_Delay();
	LCD1602_I2C_SDA_1();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：_ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			LCD1602_I2C_SDA_1();
		}
		else
		{
			LCD1602_I2C_SDA_0();
		}
		i2c_Delay();
		LCD1602_I2C_SCL_1();  //高半周期
		i2c_Delay();	
		LCD1602_I2C_SCL_0();  //低半周期
		if (i == 7)
		{
			 LCD1602_I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
}

/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* 读到第1个bit为数据的bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		LCD1602_I2C_SCL_1();
		i2c_Delay();
		if (LCD1602_I2C_SDA_READ())
		{
			value++;
		}
		LCD1602_I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	LCD1602_I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	LCD1602_I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (LCD1602_I2C_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	LCD1602_I2C_SCL_0();
	i2c_Delay();
	return re;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Ack(void)
{
	LCD1602_I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	LCD1602_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	LCD1602_I2C_SCL_0();
	i2c_Delay();
	LCD1602_I2C_SDA_1();	/* CPU释放SDA总线 */
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_NAck(void)
{
	LCD1602_I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	LCD1602_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	LCD1602_I2C_SCL_0();
	i2c_Delay();	
}

/*
*********************************************************************************************************
*	函 数 名: i2c_CfgGpio
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_CfgGpio(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(LCD1602_RCC_I2C_PORT,ENABLE); 	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Pin = LCD1602_I2C_SCL_PIN | LCD1602_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(LCD1602_GPIO_PORT_I2C, &GPIO_InitStructure);//初始化GPIO

	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	i2c_Stop();
}

/*
*********************************************************************************************************
*	函 数 名: i2c_CheckDevice
*	功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参：_Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	i2c_CfgGpio();		/* 配置GPIO */

	
	i2c_Start();		/* 发送启动信号 */

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(_Address | LCD1602_I2C_WR);
	ucAck = i2c_WaitAck();	/* 检测设备的ACK应答 */

	i2c_Stop();			/* 发送停止信号 */

	return ucAck;
}

uint8_t  i2c_WriteByte(uint8_t data)
{
	uint8_t ucAck=0;
	
	i2c_Start();		/* 发送启动信号 */
	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte( ADDRESS | LCD1602_I2C_WR);	 //寻址器件
	while( i2c_WaitAck()!=0 );   // s->m返回0寻址成功，返回1死循环
	LED2 = 0;

	i2c_SendByte(data);   //写入数据
	
	while( i2c_WaitAck()!=0 );  /*-------------fix-----------------*/
	i2c_Stop();			/* 发送停止信号 */
	
	return ucAck;

}

uint8_t i2c_R_Byte(void)
{
	uint8_t ucAck=0;
	i2c_Start();		/* 发送启动信号 */
		/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte( ADDRESS | LCD1602_I2C_RD);	 //寻址器件
	while( i2c_WaitAck()!=0 );   // s->m返回0寻址成功，返回1死循环
	
	ucAck = i2c_ReadByte();
	
	i2c_NAck();  // m->s非应答信号

	i2c_Stop();			/* 发送停止信号 */
	
	
	return ucAck;
	
	
}

