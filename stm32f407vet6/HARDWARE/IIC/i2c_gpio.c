/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��gpioģ��i2c����, ������STM32ϵ��CPU����ģ�鲻����Ӧ�ò�����֡��������I2C���߻�������������
  ******************************************************************************

  */ 


#include "i2c_gpio.h"
#include "sys.h"
#include "led.h"

//static void i2c_CfgGpio(void);



/*
*********************************************************************************************************
*	�� �� ��: i2c_Delay
*	����˵��: I2C����λ�ӳ٣����400KHz
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*��
	 	�����ʱ����ͨ���߼������ǲ��Եõ��ġ�
    ����������CPU��Ƶ72MHz ��MDK���뻷����1���Ż�
  
		ѭ������Ϊ10ʱ��SCLƵ�� = 205KHz 
		ѭ������Ϊ7ʱ��SCLƵ�� = 347KHz�� SCL�ߵ�ƽʱ��1.5us��SCL�͵�ƽʱ��2.87us 
	 	ѭ������Ϊ5ʱ��SCLƵ�� = 421KHz�� SCL�ߵ�ƽʱ��1.25us��SCL�͵�ƽʱ��2.375us 
	*/
	for (i = 0; i < 11; i++);
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C���������ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Start(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
	LCD1602_I2C_SDA_1();
	LCD1602_I2C_SCL_1();
	i2c_Delay();
	LCD1602_I2C_SDA_0();   //����
	i2c_Delay();
	LCD1602_I2C_SCL_0();   //�ͷ�SCL��
	i2c_Delay();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C����ֹͣ�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Stop(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	LCD1602_I2C_SDA_0();
	LCD1602_I2C_SCL_1();
	i2c_Delay();
	LCD1602_I2C_SDA_1();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    �Σ�_ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;

	/* �ȷ����ֽڵĸ�λbit7 */
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
		LCD1602_I2C_SCL_1();  //�߰�����
		i2c_Delay();	
		LCD1602_I2C_SCL_0();  //�Ͱ�����
		if (i == 7)
		{
			 LCD1602_I2C_SDA_1(); // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
		i2c_Delay();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_ReadByte
*	����˵��: CPU��I2C�����豸��ȡ8bit����
*	��    �Σ���
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;

	/* ������1��bitΪ���ݵ�bit7 */
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
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    �Σ���
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;

	LCD1602_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	i2c_Delay();
	LCD1602_I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	i2c_Delay();
	if (LCD1602_I2C_SDA_READ())	/* CPU��ȡSDA����״̬ */
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
*	�� �� ��: i2c_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Ack(void)
{
	LCD1602_I2C_SDA_0();	/* CPU����SDA = 0 */
	i2c_Delay();
	LCD1602_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	LCD1602_I2C_SCL_0();
	i2c_Delay();
	LCD1602_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_NAck(void)
{
	LCD1602_I2C_SDA_1();	/* CPU����SDA = 1 */
	i2c_Delay();
	LCD1602_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	LCD1602_I2C_SCL_0();
	i2c_Delay();	
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_CfgGpio
*	����˵��: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_CfgGpio(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(LCD1602_RCC_I2C_PORT,ENABLE); 	/* ��GPIOʱ�� */

	GPIO_InitStructure.GPIO_Pin = LCD1602_I2C_SCL_PIN | LCD1602_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(LCD1602_GPIO_PORT_I2C, &GPIO_InitStructure);//��ʼ��GPIO

	/* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
	i2c_Stop();
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_CheckDevice
*	����˵��: ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    �Σ�_Address���豸��I2C���ߵ�ַ
*	�� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	i2c_CfgGpio();		/* ����GPIO */

	
	i2c_Start();		/* ���������ź� */

	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte(_Address | LCD1602_I2C_WR);
	ucAck = i2c_WaitAck();	/* ����豸��ACKӦ�� */

	i2c_Stop();			/* ����ֹͣ�ź� */

	return ucAck;
}

uint8_t  i2c_WriteByte(uint8_t data)
{
	uint8_t ucAck=0;
	
	i2c_Start();		/* ���������ź� */
	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte( ADDRESS | LCD1602_I2C_WR);	 //Ѱַ����
	while( i2c_WaitAck()!=0 );   // s->m����0Ѱַ�ɹ�������1��ѭ��
	LED2 = 0;

	i2c_SendByte(data);   //д������
	
	while( i2c_WaitAck()!=0 );  /*-------------fix-----------------*/
	i2c_Stop();			/* ����ֹͣ�ź� */
	
	return ucAck;

}

uint8_t i2c_R_Byte(void)
{
	uint8_t ucAck=0;
	i2c_Start();		/* ���������ź� */
		/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte( ADDRESS | LCD1602_I2C_RD);	 //Ѱַ����
	while( i2c_WaitAck()!=0 );   // s->m����0Ѱַ�ɹ�������1��ѭ��
	
	ucAck = i2c_ReadByte();
	
	i2c_NAck();  // m->s��Ӧ���ź�

	i2c_Stop();			/* ����ֹͣ�ź� */
	
	
	return ucAck;
	
	
}
