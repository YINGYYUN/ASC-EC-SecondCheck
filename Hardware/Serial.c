#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

//发送可以直接在主函数调用Serial_SendString
//缓冲区
char Serial_RxPacket[100];

uint8_t Serial_RxFlag;

//HEX数据包，包头为FF，载荷数据固定4字节，包尾为FE

void Serial_Init(void)
{
	//初始化流程
	//1.打开USART和GPIO的时钟
	//2.GPIO初始化，把TX配置成复用输出，RX配置成输入
	//3.配置USART，使用结构体配置
	//4.若只要发送，直接开启USART;
	//若还要接收，可能需要配置中断，在开启
	//USART之前，加上ITConfig和NVIC的代码
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	//TX引脚是USART外设控制的输出脚，所以要选复用推挽输出
	//RX引脚是USART外设数据输入脚，所以要选择输入模式
	//一般RX配置是浮空输入或者上拉输入

	//把PA9配置为复用推挽输出，供USART1的TX使用（发送）
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//把PA10配置为上拉输入（接收）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;//波特率(直接写)
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制：无流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//串口模式：发送+接收
	USART_InitStructure.USART_Parity = USART_Parity_No;//校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长
	USART_Init(USART1,&USART_InitStructure);
	
	//开启中断
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	//
	
	USART_Cmd(USART1, ENABLE);
}
	
//发送数据的函数（一个字节）
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);
	//USART_FLAG_TXE发送数据位空标志位,不需要手动清零
	while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	
		
	
}

//(封装)发送数组
void Serial_SendArray(uint8_t *Array,uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

//发送字符串
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

//次方函数,返回X^Y
uint32_t Serial_Pow(uint32_t X,uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

//发送字符串形式的数字
void Serial_SendNumber(uint32_t Number,uint8_t Length)
{
	//需要拆开数字
	uint8_t i ;
	for (i = 0;i < Length; i ++) 
	{
		Serial_SendByte(Number / Serial_Pow(10,Length - i -1) % 10 + '0');
		//'0':加偏移的原因是，ascll码里数字是从0x30开始的，
		//但是此时代码中的数字是从0开始的
	}
}

//printf模拟（重定向fputc,printf的底层）
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

//封装sprintf函数
void Serial_Printf(char*format, ...)
{
	char String[100];
	//定义参数表变量
	va_list arg;
	//从format位置开始接收参数表，放在arg里面
	va_start(arg, format);
	vsprintf(String, format, arg);
	//释放参数表
	va_end(arg);
	Serial_SendString(String);
}

//中断函数
void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;//指示状态
	static uint8_t pRxPacket = 0;//指示接收位数
	if (USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		
		if (RxState == 0)//进入等待包头程序
		{
			if (RxData == '@' && Serial_RxFlag == 0)//防止发包过快
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)//进入接收数据程序
		{
			//数据长度不确定，先判断是不是包尾
			if (RxData == '\r')//第一个包尾
			{
				RxState = 2;	
			}
			else
			{
				Serial_RxPacket[pRxPacket] = RxData;//数据进入缓存数组
				pRxPacket ++;
			}
		}
		else if (RxState == 2)//进入等待包尾程序
		{
			if(RxData == '\n')//第二个包尾
			{
				RxState = 0;
				//加字符串结束标志位，方便处理
				Serial_RxPacket[pRxPacket] = '\0';
				Serial_RxFlag = 1;//接收标志位
			}
		}
		//顺手清一下DR的标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}
