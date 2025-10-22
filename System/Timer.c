#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
	//APB2外设TIM1负责定时中断
	//定时时间暂定为1ms
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	TIM_InternalClockConfig(TIM1);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up ;
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1 ;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1 ;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0 ;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM1,TIM_FLAG_Update);
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitTyStructure;
	NVIC_InitTyStructure.NVIC_IRQChannel = TIM1_UP_IRQn ;//更新中断
	NVIC_InitTyStructure.NVIC_IRQChannelCmd = ENABLE ;
	NVIC_InitTyStructure.NVIC_IRQChannelPreemptionPriority = 2 ;
	NVIC_InitTyStructure.NVIC_IRQChannelSubPriority = 1 ;
	NVIC_Init(&NVIC_InitTyStructure);
	
	TIM_Cmd(TIM1,ENABLE);
}	

//中断函数模板
/*
void TIM1_UP_IRQHandler(void)
{
	//检查标志位
	if (TIM_GetITStatus(TIM1,TIM_IT_Update) == SET )
	{
		
		//清除标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
*/
