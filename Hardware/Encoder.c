#include "stm32f10x.h"

// 二号电机编码器初始化（PA6=TIM3_CH1，PA7=TIM3_CH2）
void Encoder2_Init(void)
{
    // 1. 开启时钟（TIM3和GPIOA）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 2. 配置GPIO（PA6、PA7为上拉输入）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 3. 配置时基单元（仅用于计数范围，编码器模式托管时钟）
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;  // 最大计数，避免溢出
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;    // 不分频，保证精度
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
    
    // 4. 配置输入捕获（滤波抗干扰）
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    // 配置CH1（PA6，编码器A相）
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;  // 最大滤波，过滤高频噪声
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    // 配置CH2（PA7，编码器B相）
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    // 5. 配置编码器模式（CH1+CH2正交解码）
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // 6. 启动定时器
    TIM_Cmd(TIM3, ENABLE);
}

// 一号电机编码器初始化（PB6=TIM4_CH1，PB7=TIM4_CH2）
void Encoder1_Init(void)
{
    // 1. 开启时钟（TIM4和GPIOB）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // 2. 配置GPIO（PB6、PB7为上拉输入）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 3. 配置时基单元（与TIM3一致）
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
    
    // 4. 配置输入捕获（滤波抗干扰）
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    // 配置CH1（PB6，编码器A相）
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    // 配置CH2（PB7，编码器B相）
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    
    // 5. 配置编码器模式（CH1+CH2正交解码）
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // 6. 启动定时器
    TIM_Cmd(TIM4, ENABLE);
}

// 统一初始化函数（主函数调用）
void Encoder_Init(void)
{
    Encoder1_Init();  // 一号电机编码器（PB6/PB7+TIM4）
    Encoder2_Init();  // 二号电机编码器（PA6/PA7+TIM3）
}

// 读取一号电机编码器计数值（TIM4）
int16_t Encoder1_Get(void)
{
    int16_t temp = TIM_GetCounter(TIM4);
    TIM_SetCounter(TIM4, 0);  // 读取后清零，便于计算单位时间转速
    return temp;
}

// 读取二号电机编码器计数值（TIM3）
int16_t Encoder2_Get(void)
{
    int16_t temp = TIM_GetCounter(TIM3);
    TIM_SetCounter(TIM3, 0);
    return temp;
}
