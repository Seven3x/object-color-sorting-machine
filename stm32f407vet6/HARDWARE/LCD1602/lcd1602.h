#ifndef __LCD1602_H
#define __LCD1602_H

#include "delay.h"
#include "sys.h"
#include "main.h"
#include "stdint.h"
#include "i2c_gpio.h"
#include "led.h"




// #define PCF8574_ADDR 0x20

void LCD_Init(void);
void LCD_WriteCMD(uint8_t cmd);
void LCD_EN(void);
void LCD_WriteDAT(uint8_t dat);
void Set_Position(uint8_t x,uint8_t y);
void LCD_ShowStr(uint8_t x, uint8_t y, uint8_t * str);
void LCD_DispConf(uint8_t DispON, uint8_t CursorON, uint8_t GlareON);
void LCD1602_Wait_Ready(void);
void LCD_ShowNum(uint8_t x, uint8_t y, uint8_t num);
void LCD_Clear(void);



#endif // !__LC

