#include "stm32f10x.h"                  // Device header

#include "LED.h"

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, LED_ALL_PINS);
}

//便捷调用引脚
static uint16_t LED_SearchPin(uint8_t pos)
{
    switch(pos)
    {
        case 0: return LED_PIN_0;			
        case 1: return LED_PIN_1;
        case 2: return LED_PIN_2;
        case 3: return LED_PIN_3;
        default: return 0;
    }
}
static uint16_t LED_TimeCount = 0;//计时
static uint16_t LED_Speed_Cur = LED_SPEED_LV0;//设定流水灯时间间隔
static uint8_t LED_PIN_Cur = 0;//通过LED_SearchPin（）返回对应的引脚；
static int8_t LED_Order = LED_Order_RIGHT;

void LED_SetSpeed(uint16_t Speed)//更改流水灯时间间隔
{
	if(Speed != LED_Speed_Cur)
	{
		switch(Speed)
		{
			case 0:
				LED_Speed_Cur = LED_SPEED_LV0;
				break;
			case 1:
				LED_Speed_Cur = LED_SPEED_LV1;
				break;
			case 2:
				LED_Speed_Cur = LED_SPEED_LV2;
				break;
		}
	}
}

void LED_SetOrder(int8_t Order)//更改流水灯方向
{
	if(Order == LED_Order_RIGHT || Order == LED_Order_LEFT)
	{
		LED_Order = Order ;
	}
}

void LED_Tick(void)
{
	LED_TimeCount ++;
	LED_TimeCount %= 400;//防越界（目前阈值为114*10ms）
	
	if(LED_TimeCount >= LED_Speed_Cur)
	{
		LED_TimeCount = 0;
		LED_PIN_Cur = (LED_PIN_Cur + LED_Order + LED_COUNT)%LED_COUNT;
		GPIO_SetBits(GPIOB, LED_ALL_PINS);
		GPIO_ResetBits(GPIOB, LED_SearchPin(LED_PIN_Cur));
	}
}
