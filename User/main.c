#include "stm32f10x.h"  // Device header
#include "Delay.h"
#include "Timer.h"
#include "LED.h"
#include "Key.h"
#include "OLED.h"

/*OLED测试*/
//int main(void)
//{	
//	OLED_Init();
//	
//	//江协OLED V2.0 UFT-8 演示实例
//	//字体 OLED_8X16
//	OLED_ShowString(0,0,"Hello,世界。",OLED_8X16);
//	//整数位 2 ，小数位 3 ，字体 OLED_6X8
//	OLED_ShowFloatNum(0,16,12.345,2,3,OLED_6X8);
//	//经典格式化输出
//	OLED_Printf(0,32,OLED_8X16,"Num=%d",666);
//	
//	//必要！！！使得以上函数刷新到OLED上
//	OLED_Update();
//	
//	while(1)
//	{
//
//	}
//}


/*LED测试*/
int main(void)
{
	LED_Init();
	
	while (1)
	{
		LED_ON();
		Delay_ms(500);
		LED_OFF();
		Delay_ms(500);
		LED_Turn();
		Delay_ms(500);
		LED_Turn();
		Delay_ms(500);
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
