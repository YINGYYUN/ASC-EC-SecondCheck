#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{
	// 1. 使能时钟（TIM2和GPIOA）
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 2. 配置PA1（TIM2_CH2）和PA2（TIM2_CH3）为复用推挽输出
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽（PWM输出）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;  // 同时配置PA1和PA2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 3. 配置TIM2内部时钟
	TIM_InternalClockConfig(TIM2);
	
	// 4. 配置时基单元（决定PWM频率）
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 -1;		// ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;		// PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	// 5. 配置CH2（PA1，左电机PWMA）
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);  // 初始化默认值
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  // PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  // 高电平有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  // 使能输出
	TIM_OCInitStructure.TIM_Pulse = 0;		// CCR初始值0（占空比0%）
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  // 应用到CH2
	
	// 6. 配置CH3（PA2，右电机PWMB）
	TIM_OCInitStructure.TIM_Pulse = 0;		// 右电机初始占空比0%
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);  // 应用到CH3
	
	// 7. 使能TIM2
	TIM_Cmd(TIM2, ENABLE);
}

// 设置左电机PWM占空比（PA1，TIM2_CH2）
//这是单独更改CCR值的函数
void PWM_SetCompare2(uint16_t Compare)
{
	TIM_SetCompare2(TIM2, Compare);  // 范围0~999（对应0%~100%）
}

// 设置右电机PWM占空比（PA2，TIM2_CH3）
void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM2, Compare);  // 范围0~999（对应0%~100%）
}