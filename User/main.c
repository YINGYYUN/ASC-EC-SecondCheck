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

//PID托管标志位,预设为0，在进入对应功能后选择生效
uint8_t M1_PID_ENABLE = 0, M2_PID_ENABLE = 0;

//PID模式标志位
//0 定速 ; 1 定位 ;
//M2的定位模式已经异化为跟随模式
uint8_t M1_Mode = 0, M2_Mode = 0;

float Target1, Actual1, Out1;
float Kp1 = 1, Ki1 = 0.5, Kd1 = 1;
float Error01, Error11 ,ErrorInt1;
int32_t M1_Location = 0;

float Target2, Actual2, Out2;
float Kp2 = 1, Ki2 = 0.5, Kd2 = 1;
float Error02, Error12 ,ErrorInt2;
int32_t M2_Location = 0;

/*项目中心*/
int main()
{	
	LED_Init();
	OLED_Init();
	Key_Init();
	Serial_Init();
	Motor_Init();
	//PWM在Motor模块内初始化并调用
	Encoder_Init();

	Timer_Init();

	
	/*初始化菜单及功能1：速度设置*/	
	uint8_t KeyNum;
	
	//Menu_State表示项目状态 0:电机速度设置 / 1：电机位置跟随
	uint8_t Menu_State = 0;
		
	Motor_SetPWM1(0);
	Motor_SetPWM2(0);
	
	M1_PID_ENABLE = 1;
	M2_PID_ENABLE = 0;
	
	Serial_SendString("[INFO]READY\r\n");
	if( Menu_State == 0 )Serial_SendString("[INFO]SPEED SET MODE\r\n");
	while(1)
	{
		//功能切换部分
		KeyNum = Key_GetNum();
		if(KeyNum == 1)//按键PA0按下
		{
			Menu_State = (Menu_State + 1) % 2;
			switch(Menu_State)
			{
				case 0://定速模式
					Motor_SetPWM1(0);
					Motor_SetPWM2(0);
					M1_PID_ENABLE = 1;//M1自动
					M2_PID_ENABLE = 0;//M2手动
					
					M1_Mode = 0;//M1定速
				
					//位置数据重置
					M1_Location = 0;
					M2_Location = 0;
				
					//回传当前状态为：定速模式
					Serial_SendString("[INFO]SPEED CONTROL MODE\r\n");
					break;
				
				case 1://跟随模式
					Motor_SetPWM1(0);
					Motor_SetPWM2(0);
					M1_PID_ENABLE = 0;//M1手动
					M2_PID_ENABLE = 1;//M2自动
					
					M2_Mode = 1;//M2定位
				
					//位置数据重置
					M1_Location = 0;
					M2_Location = 0;
				
					//回传当前状态为：跟随模式
					Serial_SendString("[INFO]FOLLOWING MODE\r\n");
					break;
				
				default:
					Motor_SetPWM1(0);
					Motor_SetPWM2(0);
					M1_PID_ENABLE = 0;//M1手动
					M2_PID_ENABLE = 0;//M2手动
				
					//回传当前状态为：？？？
					Serial_SendString("[INFO]???\r\n");
					break;
			}
		}
		//功能实施部分
		switch(Menu_State)
		{			
			case 0://定速模式
				OLED_Printf(0, 0, OLED_8X16,"Speed Control");
				OLED_Update();
				if (Serial_RxFlag == 1)//收到对应格式的文本信息
				{
					Serial_Printf("[INFO]Received: %s\r\n", Serial_RxPacket);//状态回传上位机
					if (strstr(Serial_RxPacket, "speed%") != NULL) {
						int16_t speed;
						// 从字符串中提取“speed%”后面的数字
						sscanf(Serial_RxPacket, "speed%%%hd", &speed);
						Target1 = speed ;
						if (Target1 >= 100)Target1 = 99;
						if (Target1 <= -100)Target1 = -99;
						Serial_Printf("[INFO]Set_Speed:%d\r\n",(int)Target1);//状态回传上位机
					} else {
						Serial_SendString("[INFO]ERROR_COMMAND\r\n");//状态回传上位机
					}
					Serial_RxFlag = 0;//重置标志位
				}
				//%f显示浮点数，+始终显示正负号，
				//4显示宽度，0表示数值前面补零，.0表示浮点数保留0位小数
				
				//暂时决定同步展示数据
				OLED_Printf(0, 16, OLED_8X16, "P1:%4.2f", Kp1);
				OLED_Printf(0, 32, OLED_8X16, "I1:%4.2f", Ki1);
				OLED_Printf(0, 48, OLED_8X16, "D1:%4.2f", Kd1);
								
				OLED_Printf(64, 16, OLED_8X16, "Tar:%+04.0f", Target1);
				OLED_Printf(64, 32, OLED_8X16, "Act:%+04.0f", Actual1);
				OLED_Printf(64, 48, OLED_8X16, "Out:%+04.0f", Out1);
				
				OLED_Update();
				
				Serial_Printf("%f,%f,%f\r\n", Target1, Actual1, Out1);		
							
				break;
			case 1://跟随模式
				OLED_Printf(0, 0, OLED_8X16,"Following Mode");
				OLED_Update();
			
				//主要功能由PID托管
			
				//暂时决定同步展示数据
				OLED_Printf(0, 16, OLED_8X16, "P2:%4.2f", Kp2);
				OLED_Printf(0, 32, OLED_8X16, "I2:%4.2f", Ki2);
				OLED_Printf(0, 48, OLED_8X16, "D2:%4.2f", Kp2);
			
				OLED_Printf(64, 16, OLED_8X16, "1:%+06.0f", M1_Location);
				OLED_Printf(64, 32, OLED_8X16, "2:%+06.0f", M2_Location);
				OLED_Printf(64, 48, OLED_8X16, "Out:%+04.0f", Out2);
			
				OLED_Update();
			
				Serial_Printf("%d,%d,%f\r\n", (int)M1_Location, (int)M2_Location, Out2);
						
				break;
			
			default://????模式
				//留一手
				//权当一个彩蛋
				OLED_ShowString(0, 0, "GI Nod-Krai ???",OLED_8X16);
				OLED_Update();
							
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
		
		if(Count >= 10)
		{

			Count = 0;
			
			Actual1 = Encoder1_Get();
			Actual2 = Encoder2_Get();
			
			M1_Location += Actual1;
			M2_Location += Actual2;

			if (M1_PID_ENABLE )
			{
				if (M1_Mode == 0)//定速模式
				{
					Error11 = Error01;
					Error01 = Target1 - Actual1;
					
					/*输入死区*/
//					if (fabs(Error01) < 死区阈值)
//					{
//						Out1 = 0;
//					}
					//调试时防Ki变非零时调控过猛
//					else
//					{
						if ( fabs(Ki1) > EPSILON )
						{
							ErrorInt1 += Error01;
						}
						else
						{
							ErrorInt1 = 0;	
						}
						
						Out1 = Kp1 * Error01 + Ki1 * ErrorInt1 + Kd1 * (Error01 - Error11);						
//					}
					
					if(Out1 >= 100) {Out1 = 99;}
					if(Out1 <= -100) {Out1 = -99;}
					
					Motor_SetPWM1(Out1);
				}
				else if (M1_Mode == 1)
				{
					
				}
			}
			
			if (M2_PID_ENABLE)
			{
				if (M2_Mode == 0)//定速模式
				{
					
				}				
				else if (M2_Mode == 1)//定位（跟随）模式
				{
					Error12 = Error02;
					Error02 = M1_Location - M2_Location;
					
					//调试时防Ki变非零时调控过猛
					if ( fabs(Ki2) > EPSILON )
					{
						ErrorInt2 += Error02;
					}
					else
					{
						ErrorInt2 = 0;	
					}
					
					Out2 = Kp2 * Error02 + Ki2 * ErrorInt2 + Kd2 * (Error02 - Error12);
					
					if(Out2 >= 100) {Out2 = 99;}
					if(Out2 <= -100) {Out2 = -99;}
					
					Motor_SetPWM2(Out2);
				}
			}
		}
		//用于Key模块的内部检测
		Key_Tick();
		//清除标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}

