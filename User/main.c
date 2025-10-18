#include "stm32f10x.h"  // Device header
#include <string.h>

#include "Delay.h"
#include "Timer.h"
#include "LED.h"
#include "Key.h"
#include "OLED.h"
#include "Encoder.h"

#define M_Main				0
#define	M_LED_Control 		1
#define M_PID				2
#define M_Image				3
#define M_Angle				4

//容纳测试点
uint16_t test_i=0;

int main(void)
{	
	LED_Init();
	OLED_Init();
	Key_Init();
	Timer_Init();
	Encoder_Init();
	
	char Menu_Main[][12] = {"LED Control", "PID", "Image", "Angle"};
	char Menu_LED_Control[][12] = {"LED_speed", "LED_dir"};
	int8_t Value_LED_Control[] = {0,1};
	uint8_t LED_Speed_Is_Rising = 0;
	char Menu_PID[][16] = {"kp           .", "ki           .", "kd           ."};
	int16_t Value_PID[] = {0,0,0};
	char Menu_Image[][6] = {"Image"};
	char Menu_Angle[][6] = {"Angle"};
	
	//菜单层: Mian = 0;LED Control = 1;PID = 2;Image = 3;Angle = 4;
	uint8_t Flag_Menu = M_Main;
	
	//光标选择
	uint8_t Loc_Main = 0;//父菜单
	uint8_t	Loc_Sec = 0;//子菜单（二级）
	
	//选中模式: 0 否;1 是;
	uint8_t Flag_Edit = 0;
	
	//OLED显示初始化
	OLED_Clear();
	OLED_ShowString(1,2,Menu_Main[0]);
	OLED_ShowString(2,2,Menu_Main[1]);
	OLED_ShowString(3,2,Menu_Main[2]);
	OLED_ShowString(4,2,Menu_Main[3]);
	OLED_ShowString(Loc_Main+1,1,">");
	
	while(1)
	{
		//非选中模式
		if (Flag_Edit == 0)
		{
			if (Key_Check(KEY_NAME_UP,KEY_SINGLE))
			{
				switch(Flag_Menu)
				{
					case M_Main:
						OLED_ShowChar(Loc_Main+1,1,' ');
						Loc_Main = (Loc_Main-1+4)%4;
						OLED_ShowChar(Loc_Main+1,1,'>');
						break;					
					case M_LED_Control:
						OLED_ShowChar(Loc_Sec+2,1,' ');
						Loc_Sec = (Loc_Sec-1+2)%2;
						OLED_ShowChar(Loc_Sec+2,1,'>');
						break;			
					case M_PID:
						OLED_ShowChar(Loc_Sec+2,1,' ');
						Loc_Sec = (Loc_Sec-1+3)%3;
						OLED_ShowChar(Loc_Sec+2,1,'>');
						break;			
					case M_Image:
						Loc_Sec = 0;
						break;			
					case M_Angle:
						Loc_Sec = 0;
						break;
					default:
						//留一手
						break;
				}
			}
			else if (Key_Check(KEY_NAME_DOWN,KEY_SINGLE))
			{
				switch(Flag_Menu)
				{
					case M_Main:
						OLED_ShowChar(Loc_Main+1,1,' ');
						Loc_Main = (Loc_Main+1)%4;
						OLED_ShowChar(Loc_Main+1,1,'>');
						break;				
					case M_LED_Control:
						OLED_ShowChar(Loc_Sec+2,1,' ');
						Loc_Sec = (Loc_Sec+1)%2;
						OLED_ShowChar(Loc_Sec+2,1,'>');
						break;				
					case M_PID:
						OLED_ShowChar(Loc_Sec+2,1,' ');
						Loc_Sec = (Loc_Sec+1)%3;
						OLED_ShowChar(Loc_Sec+2,1,'>');
						break;				
					case M_Image:					
						Loc_Sec = 0;				
						break;			
					case M_Angle:
						Loc_Sec = 0;
						break;
					default:
						//留一手
						break;					
				}
			}
			//进入子菜单以及位于子菜单
			else if (Key_Check(KEY_NAME_COMFIRM,KEY_SINGLE))
			{
				if (Flag_Menu == M_Main)
				{
					Flag_Menu = Loc_Main + 1;
					Loc_Sec = 0;
					OLED_Clear();
					switch (Flag_Menu)
					{
						case M_LED_Control:
							OLED_ShowString(1,1,"LED Control");
							OLED_ShowString(2,2,Menu_LED_Control[0]);
							OLED_ShowString(3,2,Menu_LED_Control[1]);
							OLED_ShowChar(Loc_Sec+2,1,'>');
							OLED_ShowNum(2,14,Value_LED_Control[0],1);
							OLED_ShowSignedNum(3,13,Value_LED_Control[1],1);
							break;
						case M_PID:
							OLED_ShowString(1,1,"PID");
							OLED_ShowString(2,2,Menu_PID[0]);
							OLED_ShowString(3,2,Menu_PID[1]);
							OLED_ShowString(4,2,Menu_PID[2]);
							OLED_ShowChar(Loc_Sec+2,1,'>');
						
							int16_t kp = Value_PID[0];
							if (kp < 0)
							{
								OLED_ShowChar(2,12,'-');
								kp = -kp ;
							}
							else OLED_ShowChar(2,12,' ');
							
							OLED_ShowNum(2,14,kp/10%10,1);
							OLED_ShowNum(2,16,kp%10,1);
							if(kp/100)OLED_ShowNum(2,13,kp/100%10,1);
							
							int16_t ki = Value_PID[1];
							if (ki < 0)
							{
								OLED_ShowChar(3,12,'-');
								ki = -ki ;
							}
							else OLED_ShowChar(3,12,' ');
							
							OLED_ShowNum(3,14,ki/10%10,1);
							OLED_ShowNum(3,16,ki%10,1);
							if(ki/100)OLED_ShowNum(3,13,ki/100%10,1);
							
							int16_t kd = Value_PID[2];
							if (kd < 0)
							{
								OLED_ShowChar(4,12,'-');
								kd = -kd ;
							}
							else OLED_ShowChar(4,12,' ');
							
							OLED_ShowNum(4,14,kd/10%10,1);
							OLED_ShowNum(4,16,kd%10,1);
							if(kd/100)OLED_ShowNum(4,13,kd/100%10,1);					
							
							break;
						case M_Image:
							OLED_ShowString(1,1,"Image");
							OLED_ShowString(2,2,Menu_Image[0]);
							OLED_ShowChar(Loc_Sec+2,1,'>');
							break;
						case M_Angle:
							OLED_ShowString(1,1,"Angle");
							OLED_ShowString(2,2,Menu_Angle[0]);
							OLED_ShowChar(Loc_Sec+2,1,'>');
							break;
					}
				}
				else if (Flag_Menu == M_LED_Control || Flag_Menu == M_PID)
				{
					Flag_Edit = 1;//进入选中模式
					OLED_ShowChar(1,16,'E');
				}
				
			}
			//返回父菜单
			else if (Key_Check(KEY_NAME_BACK,KEY_SINGLE))
			{
				if (Flag_Menu != M_Main)
				{
					Flag_Menu = M_Main;
					OLED_Clear();
					OLED_ShowString(1,2,Menu_Main[0]);
					OLED_ShowString(2,2,Menu_Main[1]);
					OLED_ShowString(3,2,Menu_Main[2]);
					OLED_ShowString(4,2,Menu_Main[3]);
					OLED_ShowString(Loc_Main+1,1,">");
				}
				
			}
		}
		//选中模式
		else if (Flag_Edit == 1)
		{
			switch(Flag_Menu)
			{
				case M_LED_Control:
					if (Key_Check(KEY_NAME_COMFIRM,KEY_SINGLE))
					{
						Flag_Edit = 0;//退出选中模式
						OLED_ShowChar(1,16,' ');
					}
					else if (Loc_Sec == 0)//LED速度
					{
						if (Key_Check(KEY_NAME_UP,KEY_SINGLE)) 
						{
							if (Value_LED_Control[0] == 2 && LED_Speed_Is_Rising)
							{
							Value_LED_Control[0] = 0;
							LED_Speed_Is_Rising = 0;
							LED_SetSpeed(Value_LED_Control[0]);
							OLED_ShowNum(2,14,Value_LED_Control[0],1);
							}
							else
							{
								Value_LED_Control[0] = (Value_LED_Control[0] - 1 +3) % 3;
								LED_Speed_Is_Rising = 0;
								LED_SetSpeed(Value_LED_Control[0]);
								OLED_ShowNum(2,14,Value_LED_Control[0],1);
							}
						}
						else if (Key_Check(KEY_NAME_DOWN,KEY_SINGLE))
						{
							Value_LED_Control[0] = (Value_LED_Control[0] + 1) % 3;
							LED_Speed_Is_Rising = 1;
							LED_SetSpeed(Value_LED_Control[0]);
							OLED_ShowNum(2,14,Value_LED_Control[0],1);
						}
					}
					else if (Loc_Sec == 1)//LED方向
					{
						if (Key_Check(KEY_NAME_UP,KEY_SINGLE) || Key_Check(KEY_NAME_DOWN,KEY_SINGLE))
						{
							Value_LED_Control[1]  = Value_LED_Control[1]*-1 ;
							LED_SetOrder(Value_LED_Control[1]);
							OLED_ShowSignedNum(3,13,Value_LED_Control[1],1);
						}
					}
					break;
				case M_PID:
					Value_PID[Loc_Sec] += Encoder_Get();
					
					if (Value_PID[Loc_Sec] < -999) Value_PID[Loc_Sec] = -999;
					if (Value_PID[Loc_Sec] > 999) Value_PID[Loc_Sec] = 999;
				
					if (Key_Check(KEY_NAME_COMFIRM,KEY_SINGLE))
					{
						Flag_Edit = 0;//退出选中模式
						OLED_ShowChar(1,16,' ');
					}else if (Key_Check(KEY_NAME_UP,KEY_SINGLE) || Key_Check(KEY_NAME_UP,KEY_REPEAT))
					{
						Value_PID[Loc_Sec] ++;
					}
					else if (Key_Check(KEY_NAME_DOWN,KEY_SINGLE) || Key_Check(KEY_NAME_DOWN,KEY_REPEAT))
					{
						Value_PID[Loc_Sec] --;
					}
					
					if (Value_PID[Loc_Sec] < -999) Value_PID[Loc_Sec] = -999;
					if (Value_PID[Loc_Sec] > 999) Value_PID[Loc_Sec] = 999;
					
					int16_t current_pid = Value_PID[Loc_Sec];//神秘Warning
					
					if (current_pid < 0) {
						OLED_ShowChar(Loc_Sec+2, 12, '-');
						current_pid = -current_pid;
					}
					else OLED_ShowChar(Loc_Sec+2, 12, ' ');
					
					OLED_ShowNum(Loc_Sec+2,14,current_pid/10%10,1);							
					OLED_ShowNum(Loc_Sec+2,16,current_pid%10,1);
					if(current_pid/100%10 == 0)OLED_ShowChar(Loc_Sec+2,13,' ');					
					if(current_pid/100)OLED_ShowNum(Loc_Sec+2,13,current_pid/100%10,1);	
					
					break;
				default:
					//留一手
					break;
			}
		}
	}		
}

//10ms的定时中断
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2,TIM_IT_Update) == SET )
	{
		//由定时器中断自动执行;有利于多模块共用定时器定时
		//同时，需要防止中断重叠
		//一:减小模块内中断函数的内容，减小运行时间
		//二：增加定时器的基础时间
		Key_Tick();
		LED_Tick();
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
