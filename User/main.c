#include "stm32f10x.h"  // Device header
#include "Delay.h"
#include "Timer.h"
#include "LED.h"
#include "OLED.h"
#include "Key.h"
#include "Serial.h"
#include "Motor.h"
#include "Encoder.h"
#include <string.h>

int16_t Speed1;
int16_t Location1;

/*项目中心*/
int main()
{
	
	LED_Init();
	OLED_Init();
	Key_Init();
	Serial_Init();
	Motor_Init();
	Encoder_Init();

	Timer_Init();
	//PWM在Motor模块内初始化并调用
	
	uint8_t KeyNum;
	uint8_t Menu_State = 0;
	//Menu_State表示项目状态 0:电机速度设置 / 1：电机速度同步
	
	Motor_SetPWM1(0);
	Motor_SetPWM2(0);
//	OLED_ShowString(0, 0, "ABCDEFGHIJKLMNOP",OLED_8X16);
//	OLED_ShowString(0, 48, "A",OLED_8X16);
//	OLED_ShowString(120, 48, "B",OLED_8X16);
//	
//	OLED_Update();
	
	Serial_SendString("[INFO]READY\r\n");
	if( Menu_State == 0 )Serial_SendString("[INFO]SPEED SET MODE\r\n");
	while(1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum == 1)//按键按下
		{
			Menu_State = (Menu_State + 1) % 2;
			switch(Menu_State)
			{
				case 0:
					//回传当前状态为：速度设置
					Serial_SendString("[INFO]SPEED SET MODE\r\n");
					break;
				
				case 1:
//					Motor_SetPWM1(0);
//					Motor_SetPWM2(0);
					//回传当前状态为：速度同步
					Serial_SendString("[INFO]SPEED SYNC MODE\r\n");
					break;
				
				default:
					//回传当前状态为：？？？
					Serial_SendString("[INFO]???\r\n");
					break;
			}
		}

		switch(Menu_State)
		{
			
			case 0:
				if (Serial_RxFlag == 1)//收到对应格式的文本信息
				{
					Serial_Printf("[INFO]Received: %s\r\n", Serial_RxPacket);
					if (strstr(Serial_RxPacket, "speed%") != NULL) {
						int16_t speed;
						// 从字符串中提取“speed%”后面的数字
						sscanf(Serial_RxPacket, "speed%%%hd", &speed);
						if (speed >= 100)speed = 99;
						if (speed <= -100)speed = -99;
						Serial_Printf("[INFO]Set_Speed:%d\r\n",speed);
						Motor_SetPWM1(speed);
//						Motor_SetPWM2(speed);
					} else {
						Serial_SendString("[INFO]ERROR_COMMAND\r\n");
					}
					Serial_RxFlag = 0;//重置标志位
				}
				OLED_Printf(0,0,OLED_8X16,"Speed1:%+05d",Speed1);
				OLED_Printf(0,16,OLED_8X16,"Location1:%+05d",Location1);
				OLED_Update();
				
				
				
				
				
			
				break;
			case 1:
				OLED_ShowString(0, 0, "222",OLED_8X16);
				OLED_Update();
			
			
				break;
			
			default:
				OLED_ShowString(0, 0, "GI Nod-Krai ???",OLED_8X16);
				OLED_Update();
				//留一手
				break;	
			
			
			
		}
	}
}





//由定时器中断自动执行;有利于多模块共用定时器定时
		//同时，需要防止中断重叠
		//一:减小模块内中断函数的内容，减小运行时间
		//二：增加定时器的基础时间
//1ms定时器
void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count;
	
	//检查标志位
	if (TIM_GetITStatus(TIM1,TIM_IT_Update) == SET )
	{
		Count++;
		if(Count >= 40)
		{
			Count = 0;
			
			Speed1 = Encoder1_Get();
			Location1 += Speed1;
			Serial_Printf("%.2f\n", Speed1);
		}
		//用于Key模块的内部检测
		Key_Tick();
		//清除标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}

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

/*TIM1定时器定时中断和非阻塞式按键测试*/
//uint16_t i = 0;
//uint16_t j = 0;
//uint8_t KeyNum;

//int main(void)
//{
//	OLED_Init();
//	Key_Init();
//	
//	Timer_Init();
//	
//	while (1)
//	{
//		KeyNum = Key_GetNum();
//		if(KeyNum == 1)//PA-按下
//		{
//			j++;
//		}
//		OLED_Printf(0,0,OLED_8X16,"i:%05d",i);
//		OLED_Printf(0,16,OLED_8X16,"i:%05d",j);
//		
//		OLED_Update();
//	}
//}

////由定时器中断自动执行;有利于多模块共用定时器定时
//		//同时，需要防止中断重叠
//		//一:减小模块内中断函数的内容，减小运行时间
//		//二：增加定时器的基础时间
//void TIM1_UP_IRQHandler(void)
//{
//	//检查标志位
//	if (TIM_GetITStatus(TIM1,TIM_IT_Update) == SET )
//	{
//		i++;
//		
//		//用于Key模块的内部检测
//		Key_Tick();
//		//清除标志位
//		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
//	}
//}

/*串口接收文本测试*/

//int main()
//{
//	OLED_Init();
//	LED_Init();
//	Serial_Init();
//	
//	OLED_ShowString(0, 0,"TxPacket",OLED_8X16);
//	OLED_ShowString(0, 32,"RxPacket",OLED_8X16);
//	OLED_Update();
//	
//	while(1)
//	{
//		if (Serial_RxFlag == 1)
//		{
//			OLED_ShowString(0, 48, "                ",OLED_8X16);
//			OLED_ShowString(0, 48, Serial_RxPacket,OLED_8X16);
//			OLED_Update();
//			
//			if (strcmp(Serial_RxPacket,"LED_ON") == 0)//判断指令
//			{
//				LED_ON();
//				Serial_SendString("LED_ON_OK\r\n"); //回传
//				OLED_ShowString(0, 16, "                ",OLED_8X16);
//				OLED_ShowString(0, 16, "LED_ON_OK",OLED_8X16);//显示
//				OLED_Update();
//			}
//			else if (strcmp(Serial_RxPacket,"LED_OFF") == 0)//判断指令
//			{
//				LED_OFF();
//				Serial_SendString("LED_OFF_OK\r\n"); //回传
//				OLED_ShowString(0, 16, "                ",OLED_8X16);
//				OLED_ShowString(2, 16, "LED_OFF_OK",OLED_8X16);//显示
//				OLED_Update();
//			}
//			else
//			{
//				Serial_SendString("ERROR_COMMAND\r\n"); //回传
//				OLED_ShowString(0, 16, "                ",OLED_8X16);
//				OLED_ShowString(0, 16, "ERROR_COMMAND",OLED_8X16);//显示
//				OLED_Update();
//			}
//			
//			Serial_RxFlag = 0;
//		}
//	}
//}
