#include "stm32f10x.h"  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Timer.h"

uint16_t Num1, Num2;

int main(void)
{	
	OLED_Init();
	Key_Init();
	Timer_Init();
	
	OLED_ShowString(1, 1, "Num1:");
	OLED_ShowString(2, 1, "Num2:");
	
	while(1)
	{
		/*示例1*/
//		if (Key_Check(KEY_HOLD))//按下B1时
//		{
//			Num1 = 1;
//		}
//		else
//		{
//			Num1 = 0;
//		}
		
		/*示例2*/
//		if (Key_Check(KEY_DOWN))//按下按键瞬间
//		{
//			Num1 ++;
//		}
//		if (Key_Check(KEY_UP))//松开按键瞬间
//		{
//			Num2 ++;
//		}
		
		/*示例3*/
//		if (Key_Check(KEY_SINGLE))
//		{
//			Num1 ++;
//		}
//		if (Key_Check(KEY_DOUBLE))
//		{
//			Num1 += 100;
//		}
//		if (Key_Check(KEY_LONG))
//		{
//			Num1 = 0;
//		}
//		
		/*示例4*/
		if	(Key_Check(KEY_1,KEY_SINGLE) || Key_Check(KEY_1,KEY_REPEAT))
		{
			Num1 ++;//单击加一，长按快速自增
		}
		if	(Key_Check(KEY_2,KEY_SINGLE) || Key_Check(KEY_2,KEY_REPEAT))
		{
			Num1 --;
		}
		if	(Key_Check(KEY_3,KEY_SINGLE))
		{
			Num1 = 0;
		}
		if	(Key_Check(KEY_4,KEY_LONG))
		{
			Num1 = 9999;
		}
		
		
		OLED_ShowNum(1, 6, Num1, 5);
		OLED_ShowNum(2, 6, Num2, 5);
	}
		
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2,TIM_IT_Update) == SET )
	{
		//由定时器中断自动执行;有利于多模块共用定时器定时
		//同时，需要防止中断重叠
		//一:减小模块内中断函数的内容，减小运行时间
		//二：增加定时器的基础时间
		Key_Tick();
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
