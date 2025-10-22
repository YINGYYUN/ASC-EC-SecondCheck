#include "stm32f10x.h"                  // Device header

uint16_t LED1_Mode;//是否闪灯标志
uint16_t LED2_Mode;

uint16_t LED1_Count;//计次变量
uint16_t LED2_Count;

//PC13(测试LED)

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void LED_ON(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void LED_OFF(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void LED_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == 0)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	}
}
