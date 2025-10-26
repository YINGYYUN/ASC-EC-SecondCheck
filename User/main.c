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
#include <math.h>

#define EPSILON 0.0001

float Target1, Actual1, Out1;
float Kp1 = 10, Ki1 = 0.5, Kd1 = 1;
float Error01, Error11 ,ErrorInt1;
//float Error21;
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
		//切换
		KeyNum = Key_GetNum();
		if(KeyNum == 1)//按键PA0按下
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

		//功能
		switch(Menu_State)
		{
			
			case 0:
				OLED_Printf(0, 0, OLED_8X16,"Speed Control");
				OLED_Update();
				if (Serial_RxFlag == 1)//收到对应格式的文本信息
				{
					Serial_Printf("[INFO]Received: %s\r\n", Serial_RxPacket);
					if (strstr(Serial_RxPacket, "speed%") != NULL) {
						int16_t speed;
						// 从字符串中提取“speed%”后面的数字
						sscanf(Serial_RxPacket, "speed%%%hd", &speed);
						Target1 = speed ;
						if (Target1 >= 100)Target1 = 99;
						if (Target1 <= -100)Target1 = -99;
						Serial_Printf("[INFO]Set_Speed:%d\r\n",(int)Target1);
					} else {
						Serial_SendString("[INFO]ERROR_COMMAND\r\n");
					}
					Serial_RxFlag = 0;//重置标志位
				}
				//%f显示浮点数，+始终显示正负号，
				//4显示宽度，0表示数值前面补零，.0表示浮点数保留0位小数
				OLED_Printf(0, 16, OLED_8X16, "Kp:%+04.0f", Kp1);
				OLED_Printf(0, 32, OLED_8X16, "Ki:%+04.0f", Ki1);
				OLED_Printf(0, 48, OLED_8X16, "Kd:%+04.0f", Kd1);
								
				OLED_Printf(64, 16, OLED_8X16, "Tar:%+04.0f", Target1);
				OLED_Printf(64, 32, OLED_8X16, "Act:%+04.0f", Actual1);
				OLED_Printf(64, 48, OLED_8X16, "Out:%+04.0f", Out1);
				
				OLED_Update();
				
				Serial_Printf("%f,%f,%f\r\n", Target1, Actual1, Out1);		
				
			
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
			/*位置式PID*/
			Count = 0;
			
			Actual1 = Encoder1_Get();
			
			Error11 = Error01;
			Error01 = Target1 - Actual1;
			
			//调试时防Ki变非零时调控过猛
			if ( fabs(Ki1) > EPSILON )
			{
			ErrorInt1 += Error01;
			}
			else
			{
			ErrorInt1 = 0;	
			}
			
			Out1 = Kp1 * Error01 + Ki1 * ErrorInt1 + Kd1*(Error01 - Error11);
			
			if(Out1 >= 100) {Out1 = 99;}
			if(Out1 <= -100) {Out1 = -99;}
			
			Motor_SetPWM1(Out1);
			
			/*增量式PID*/
//			Count = 0;
//			
//			Actual1 = Encoder1_Get();
//			
//			Error21 = Error11;
//			Error11 = Error01;
//			Error01 = Target1 - Actual1;
//			
//	
//			Out1 += Kp1 * (Error01 - Error11) + Ki1 * Error01 + Kd1*(Error01 - 2 * Error11 + Error21);
//			
//			if(Out1 >= 100) {Out1 = 99;}
//			if(Out1 <= -100) {Out1 = -99;}
//			
//			Motor_SetPWM1(Out1);
		}
		//用于Key模块的内部检测
		Key_Tick();
		//清除标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}

