#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_Init(void)
{
	//普通的GPIOB输出初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	PWM_Init();
}

//设置一号电机PWM占空比和电机方向（此处开环）
void Motor_SetPWM1(int16_t PWM)
{
	int16_t pwm = PWM ; 
    if (pwm >= 100) pwm = 99;
    if (pwm <= -100) pwm = -99;
	if (pwm >= 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
		PWM_SetCompare2(pwm);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
		GPIO_SetBits(GPIOB, GPIO_Pin_13);
		PWM_SetCompare2(-pwm);
	}
}

//设置二号电机PWM占空比和电机方向（此处开环）
void Motor_SetPWM2(int16_t PWM)
{
	int16_t pwm = PWM ;
    if (pwm >= 100) pwm = 99;
    if (pwm <= -100) pwm = -99;
	if (pwm >= 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_10);
		GPIO_ResetBits(GPIOB, GPIO_Pin_11);
		PWM_SetCompare3(pwm);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_10);
		GPIO_SetBits(GPIOB, GPIO_Pin_11);
		PWM_SetCompare3(-pwm);
	}
}
