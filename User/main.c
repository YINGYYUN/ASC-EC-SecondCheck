#include "stm32f10x.h"  // Device header
#include "Delay.h"
#include "Timer.h"
#include "LED.h"
#include "OLED.h"
#include "Key.h"

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
//int main(void)
//{
//	LED_Init();
//	
//	while (1)
//	{
//		LED_ON();
//		Delay_ms(500);
//		LED_OFF();
//		Delay_ms(500);
//		LED_Turn();
//		Delay_ms(500);
//		LED_Turn();
//		Delay_ms(500);
//	}
//}

/*TIM1定时器定时中断和非阻塞式按键测试*/
uint16_t i = 0;
uint16_t j = 0;
uint8_t KeyNum;

int main(void)
{
	OLED_Init();
	Key_Init();
	
	Timer_Init();
	
	while (1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum == 1)//PA-按下
		{
			j++;
		}
		OLED_Printf(0,0,OLED_8X16,"i:%05d",i);
		OLED_Printf(0,16,OLED_8X16,"i:%05d",j);
		
		OLED_Update();
	}
}

//由定时器中断自动执行;有利于多模块共用定时器定时
		//同时，需要防止中断重叠
		//一:减小模块内中断函数的内容，减小运行时间
		//二：增加定时器的基础时间
void TIM1_UP_IRQHandler(void)
{
	//检查标志位
	if (TIM_GetITStatus(TIM1,TIM_IT_Update) == SET )
	{
		i++;
		
		//用于Key模块的内部检测
		Key_Tick();
		//清除标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}

