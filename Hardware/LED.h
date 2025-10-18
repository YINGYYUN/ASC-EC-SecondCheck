#ifndef __LED_H
#define __LED_H

// 宏定义：LED引脚（PB12~PB15）
#define LED_COUNT				4 
#define LED_PIN_0 GPIO_Pin_12
#define LED_PIN_1 GPIO_Pin_13
#define LED_PIN_2 GPIO_Pin_14
#define LED_PIN_3 GPIO_Pin_15
#define LED_ALL_PINS (LED_PIN_0 | LED_PIN_1 | LED_PIN_2 | LED_PIN_3)

//宏定义：流水灯延时挡位
#define LED_SPEED_LV0  			50        // 默认
#define LED_SPEED_LV1  			100       
#define LED_SPEED_LV2  			20   

//宏定义：流水灯方向

#define LED_Order_RIGHT  		1   	// 向右（PB12→PB13→PB14→PB15）
#define LED_Order_LEFT  		-1  	// 向左（PB15→PB14→PB13→PB12）

void LED_Init(void);
void LED_SetSpeed(uint16_t Speed);
void LED_SetOrder(int8_t Order);
void LED_Tick(void);

#endif
