#include "openmv.h"

//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART3_RX_BUF;     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART3_RX_STA=0;       //接收状态标记	

void USART3_IRQHandler(void) {//必须接受0x0d 0x0a
    u8 Res;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        Res = USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据
		// USART_SendData(USART3, Res);         //向串口3发送数据
        // while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
		if((USART3_RX_STA&0x8000)==0 )//接收未完成
		{
			if(USART3_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART3_RX_STA=0;//接收错误,重新开始
				else {
                    USART3_RX_STA|=0x8000;	//接收完成了
                    USART_SendData(USART3, 'b');         //向串口3发送数据
                    while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
                }
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d) {
                    USART3_RX_STA|=0x4000;
                    USART_SendData(USART3, 'a');         //向串口3发送数据
                    while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
                }
                else
				{
					USART3_RX_BUF=Res;

					USART3_RX_STA++;
					if(USART3_RX_STA> 1)USART3_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		} 
    }

}

void Usart3_Init(u32 bound) {
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); 
    
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //PB10复用为USART3 TX
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);//PB11复用为USART3 RX
  	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10与GPIOB11 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //速度50MHz 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉 

    GPIO_Init(GPIOB,&GPIO_InitStructure); 

    USART_InitStructure.USART_BaudRate = bound;//一般设置为9600; 
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式 
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位 
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位 
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式 
    USART_Init(USART3, &USART_InitStructure); //初始化串口 

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;    //响应优先级3 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;      //IRQ 通道使能 
    NVIC_Init(&NVIC_InitStructure);  //根据指定的参数初始化VIC寄存器、 

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断，接收到数据中断

}
void Openmv_Init(void) {
    Usart3_Init(115200);

    USART_Cmd(USART3, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    
}