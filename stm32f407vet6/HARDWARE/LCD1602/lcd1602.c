#include "lcd1602.h"

unsigned char  LCD_data = 0x08;

void LCD1602_Wait_Ready(void)
{
	uint8_t sta;  

	LCD_data |= 0xf0;     //数据高四位置1
	
	i2c_WriteByte(LCD_data);
	LCD_data &=~(1<<(1-1));//RS=0;               // 0  0  0  0      0  0  0   0
  	LCD_data |=(1<<(2-1));//RW=1;                // D7 D6 D5 D4        E  RW  RS
	i2c_WriteByte(LCD_data);	
	do
	{
		LCD_data |= 1<<2;          // 0000（数据）0000（控制位）
								   // 0xxx 0100  -------> E=1
		i2c_WriteByte(LCD_data);
		
		delay_ms(5);		//延时
		
		sta = i2c_R_Byte();       /* 读取状态字 */
		
		LCD_data ^= 1<<2;              	//异或运算（还原）
										// 0xxx 0000  ----------> E=0
									   //or  LCD_data &=~(1<<2)  						  
		//I2C_WriteByte(LCD_data);//1→0 执行指令	
		i2c_WriteByte(LCD_data);	
	}while(sta &0x80);           /* 状态字的最高位 1忙，0空闲 */ 
}




void LCD_EN(void)
{
	LCD_data |= 1<<2;          // 0000（数据）0000（控制位）
	                           // 0xxx 0100  -------> E=1
	//I2C_WriteByte(LCD_data);	//1 读取数据
	i2c_WriteByte(LCD_data);
	delay_us(10);		//延时
	LCD_data ^= 1<<2;              	//异或运算（还原）
									// 0xxx 0000  ----------> E=0
	                               //or  LCD_data &=~(1<<2)  						  
	//I2C_WriteByte(LCD_data);//1→0 执行指令	
	i2c_WriteByte(LCD_data);
	
}



void LCD_WriteCMD(uint8_t cmd)
{
	
	LCD1602_Wait_Ready();
	LCD_data &=~(1<<(1-1));//RS=0;
	LCD_data &=~(1<<(2-1));//RW=0;
	i2c_WriteByte(LCD_data);   //需要修改完整的发送数据
	
	
	LCD_data &= 0X0f; //清高四位
	LCD_data |= cmd & 0xf0; //写指令高四位
	i2c_WriteByte(LCD_data);
	LCD_EN();
	
	cmd = cmd<<4;
	LCD_data &= 0X0f; //清高四位
	LCD_data |= cmd & 0xf0; //写指令低四位
	i2c_WriteByte(LCD_data);
	LCD_EN();
}



void LCD_WriteDAT(uint8_t dat)
{
	
	LCD1602_Wait_Ready();
	LCD_data |= (1<<(1-1));			//RS=1;
	LCD_data &=~(1<<(2-1));			//RW=0;
	i2c_WriteByte(LCD_data);
	
	LCD_data &= 0X0f; 				//清高四位
	LCD_data |= dat & 0xf0; 		//写数据高四位
	i2c_WriteByte(LCD_data);
	LCD_EN();
	
	dat = dat<<4;
	LCD_data &= 0X0f; 				//清高四位
	LCD_data |= dat & 0xf0; 		//写数据低四位
	i2c_WriteByte(LCD_data);
	LCD_EN();
}


void Set_Position(uint8_t x,uint8_t y)
{
	
	uint8_t x_pos;
	x_pos = x%0x10;
	if(y==0)
	{
		LCD_WriteCMD(0x80+x_pos);
	}
	else 
	{
		LCD_WriteCMD(0xc0+x_pos);
	}
	
}


/*-----------------字符发送-------------------------------*/
void LCD_ShowStr(uint8_t x, uint8_t y, uint8_t * str)
{
	Set_Position(x,y);
	while(*str != '\0')
	{
		
		LCD_WriteDAT(*str);
		str++;
	}
	
}

/*-------------char转字符串-----------------------------*/
void LCD_ShowNum(uint8_t x, uint8_t y, uint8_t dat)
{
	uint8_t  str[10];
    str[0] = (dat/100) + '0';  //转换为十进制字符串格式
    str[1] = (dat/10%10) + '0';
    str[2] = (dat%10) + '0';
    str[3] = '\0';
	if(str[0] == '0')
	{
		LCD_ShowStr(x,y,str+1);
	}
	
	
}

void LCD_Clear(void)
{
	LCD_WriteCMD(0x01);/* 清屏    */
}


void LCD_DispConf(uint8_t DispON, uint8_t CursorON, uint8_t GlareON)
{
	uint8_t x = 0x08;
	if(DispON!=0)
	{
		x = x|(DispON<<2);
	}
	if(CursorON!=0)
	{
		x = x|(CursorON<<1);
	}
	if(GlareON!=0)
	{
		x = x|GlareON;
	}	
	LCD_WriteCMD(x);
}



void LCD_Init(void)
{                      
	i2c_CfgGpio();		/* 配置GPIO */
	
	LCD_WriteCMD(0x02);/*必须要，如果没有了第一次上电会没内容*/
	LCD_WriteCMD(0x28);/* 0x28  4线操作模式   0x38 8线模式  */  
	
	/* 0000  1DCB  D开关显示 C开关光标 B开关光标闪烁   */
	LCD_WriteCMD(0x0c);/* 开显示，关光标    */
	LCD_WriteCMD(0x06);/* 文字不动，地址自动加1   */
	LCD_WriteCMD(0x01);/* 清屏    */
	delay_ms( 5);
}

