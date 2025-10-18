#include "stm32f10x.h"                  // Device header

#include "Key.h"		//使用了位于Key.h的宏定义

//用宏定义替换1/0，便于理解Key_GetState（）
#define KEY_PRESSED 			1
#define	KEY_UNPRESSED			0

//宏定义替换时间阈值
/*
说明：由于按键检测和状态机代码每隔20ms才会执行一次
所以此处的各个时间阈值最好都设置为20ms的倍数
所以即使设定时间为50ms，但实际却会是60ms
*/
//10ms的定时中断
//取消双击判定，在此处留一处痕迹
#define KEY_Time_LONG			100
#define KEY_Time_REPEAT			10

uint8_t Key_Flag[KEY_COUNT];//不同的位表示不同的事件标志位

//考核项目要求的按键均为下拉输入
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}

//形参指定按键；返回当前按键按下状态/未按下状态
uint8_t Key_GetState(uint8_t n)
{
	if (n == KEY_NAME_UP)//0
	{
		if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4) == 0)//上键
		{
		return KEY_PRESSED;//返回按下
		}
		return KEY_UNPRESSED;//返回未按下
	}
	else if (n == KEY_NAME_DOWN)//1
	{
		if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == 0)//下键
		{
		return KEY_PRESSED;
		}
		return KEY_UNPRESSED;
	}
	else if (n == KEY_NAME_COMFIRM)//2
	{
		if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) == 0)//确认键
		{
		return KEY_PRESSED;
		}
		return KEY_UNPRESSED;
	}
	else if (n == KEY_NAME_BACK)//3
	{
		if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14) == 0)//返回键
		{
		return KEY_PRESSED;
		}
		return KEY_UNPRESSED;
	}
}

//第一个参数指定按钮
//第二个参数指定标志位;格式：0xXX
uint8_t Key_Check(uint8_t n, uint8_t Flag)
{
	//一个变量&位掩码
	//例如：变量&0x01，如果最低位是1，则结果为0x01
	//				   如果最低位是0，则结果为0x00
	//		变量&0x02，如果次低位是1，则结果为0x02
	//				   如果次低位是0，则结果为0x00
	//注意，指定位为1时，结果是位掩码本身(非0)，而不一定是0x01
	if (Key_Flag[n] & Flag)
	{
		if (Flag != KEY_HOLD)//检测指定位是否为KEY_HOLD
		{
			//清零KEY_HOLD；其他标志位在主程序读后清零
			Key_Flag[n] &= ~Flag;
		}
		return 1;//指定标志位是1
	}
	return 0;//指定标志位是0
}

void Key_Tick(void)//利用定时中断调用，获取通用的时间基准
{
	static uint8_t Count;
	static uint8_t i;
	static uint8_t CurrState[KEY_COUNT],PrevState[KEY_COUNT];//Current,Previous
	static uint8_t S[KEY_COUNT];//状态变量
	static uint16_t Time[KEY_COUNT];//长按计时器（此处递减计时）
	//静态变量默认值为0，函数退出后值不会丢失
	
	for (i = 0;i < KEY_COUNT; i ++)
	{
		if  (Time[i] > 0)
		{
			Time[i] --;
		}
	}
	Count++;//计数分频
	if (Count >= 2)//可过滤按键抖动
	{
		Count = 0 ;
		
		for (i = 0;i < KEY_COUNT; i ++)
		{
			PrevState[i] = CurrState[i];//上次状态
			CurrState[i] = Key_GetState(i);//本次状态
			
			//置标志位
			//Key_Flag |= 0x01;	
			//只把最低位置一，而不影响其他位
			//Key_Flag &= 0xFE;  或者  Key_Flag &= ~0x01;
			//只把最低位清零，而不影响其他位
			//使用宏定义转换0xXX,增加可读性
			
			if (CurrState[i] == KEY_PRESSED)
			{
				Key_Flag[i] |= KEY_HOLD;//HOLD = 1
			}
			else
			{
				Key_Flag[i] &= ~KEY_HOLD;//HOLD = 0
			}
			
			if (CurrState[i] == KEY_PRESSED && PrevState[i] == KEY_UNPRESSED)
			{
				Key_Flag[i] |= KEY_DOWN;//DOWM = 1
				//按下瞬间
				//(中断程序只需置一，由主程序置零)
			}
			
			if (CurrState[i] == KEY_UNPRESSED && PrevState[i] == KEY_PRESSED)
			{
				Key_Flag[i] |= KEY_UP;//UP = 1
				//松开瞬间
			}
			
			if (S[i] == 0)//状态：空闲
			{
				if (CurrState[i] == KEY_PRESSED)//按键按下
				{
					Time[i] = KEY_Time_LONG;//设定长按时间
					S[i] = 1;
				}
			}
			else if (S[i] == 1)//状态：按键已按下
			{		
				if (CurrState[i] == KEY_UNPRESSED)//按键松开
				{
					Key_Flag[i] |= KEY_SINGLE;//SINGLE = 1
					S[i] = 0;
				}
				else if (Time[i] == 0)//长按时间到（成功）
				{
					Time[i] = KEY_Time_REPEAT;//设定重复时间
					Key_Flag[i] |= KEY_LONG;//LONG = 1
					S[i] = 4;
				}
			}
			
			else if (S[i] == 4)//状态：按键已长按
			{
				if (CurrState[i] == KEY_UNPRESSED)
				{
					S[i] = 0;
				}
				else if (Time[i] == 0)
				{
					Time[i] = KEY_Time_REPEAT;//设定重复时间
					Key_Flag[i] |= KEY_REPEAT;//REPEAT = 1
					S[i] = 4;
				}
			}
		}
		
	}
}
