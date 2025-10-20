#include "stm32f10x.h"                  // Device header

uint16_t LED1_Mode;//是否闪灯标志
uint16_t LED2_Mode;

uint16_t LED1_Count;//计次变量
uint16_t LED2_Count;

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);
}

void LED1_SetMode(uint8_t Mode)//更改闪灯标志
{
	if(Mode != LED1_Mode)
	{
		LED1_Mode = Mode ;
		LED1_Count = 0 ;
	}
}

void LED2_SetMode(uint8_t Mode)//更改闪灯标志
{
	if(Mode != LED2_Mode)
	{
		LED2_Mode = Mode ;
		LED2_Count = 0 ;
	}
}

void LED1_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}

void LED1_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void LED2_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);
}

void LED2_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
}

void LED_Tick(void)
{
	if( LED1_Mode == 0 )
	{	
		LED1_OFF();
	}
	else if( LED1_Mode == 1 )
	{
		LED1_ON();
	}
	else if( LED1_Mode == 2 )//慢闪
	{
		LED1_Count++;
		LED1_Count %= 1000 ;//计数周期防越界
//		if( LED1_Count>=1000 )LED1_Count = 0 ;
			
		if( LED1_Count<500 )
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
	}
	else if( LED1_Mode == 3 )//快闪
	{
		LED1_Count++;
		LED1_Count %= 100 ;
			
		if( LED1_Count<50 )
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
	}
	else if( LED1_Mode == 4 )//点闪
	{
		LED1_Count++;
		LED1_Count %= 1000 ;
			
		if( LED1_Count<100 )
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
	}
	
	if( LED2_Mode == 0 )
	{	
		LED2_OFF();
	}
	else if( LED2_Mode == 1 )
	{
		LED2_ON();
	}
	else if( LED2_Mode == 2 )//慢闪
	{
		LED1_Count++;
		LED1_Count %= 1000 ;//计数周期防越界
//		if( LED1_Count>=1000 )LED1_Count = 0 ;
			
		if( LED2_Count<500 )
		{
			LED2_ON();
		}
		else
		{
			LED2_OFF();
		}
	}
	else if( LED2_Mode == 3 )//快闪
	{
		LED2_Count++;
		LED2_Count %= 100 ;
			
		if( LED2_Count<50 )
		{
			LED2_ON();
		}
		else
		{
			LED2_OFF();
		}
	}
	else if( LED2_Mode == 4 )//点闪
	{
		LED2_Count++;
		LED2_Count %= 1000 ;
			
		if( LED2_Count<100 )
		{
			LED2_ON();
		}
		else
		{
			LED2_OFF();
		}
	}
}
