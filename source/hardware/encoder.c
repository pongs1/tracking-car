#include "stm32f10x.h"

/**
  * @brief  初始化右侧编码器(TIM4)
  */
void encoder_init_right() {
    // 使能GPIOB和TIM4的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // 配置GPIO端口
    GPIO_InitTypeDef GPIO_Init_encoderStr;
    GPIO_Init_encoderStr.GPIO_Mode = GPIO_Mode_IPU;  // 设置输入模式为上拉输入
    GPIO_Init_encoderStr.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // 配置引脚6和7
    GPIO_Init_encoderStr.GPIO_Speed = GPIO_Speed_50MHz;  // 设置端口速度
    GPIO_Init(GPIOB, &GPIO_Init_encoderStr);  // 初始化GPIO端口

    // 设置TIM4的时钟源为内部时钟
    TIM_InternalClockConfig(TIM4);

    // 初始化TIM4的计数器和时基设置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInit_TIM4Str;
    TIM_TimeBaseInit_TIM4Str.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟分频
    TIM_TimeBaseInit_TIM4Str.TIM_RepetitionCounter = 0;  // 重复计数器
    TIM_TimeBaseInit_TIM4Str.TIM_CounterMode = TIM_CounterMode_Up;  // 计数器模式为向上计数
    TIM_TimeBaseInit_TIM4Str.TIM_Prescaler = 0;  // 预分频器
    TIM_TimeBaseInit_TIM4Str.TIM_Period = 65535;  // 计数器周期
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInit_TIM4Str);  // 初始化TIM4的时基设置

    // 初始化TIM4的编码器输入模式
    TIM_ICInitTypeDef TIM_ICInitStr;
    TIM_ICStructInit(&TIM_ICInitStr);
    TIM_ICInitStr.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStr.TIM_ICFilter = 0xf;
    TIM_ICInitStr.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInit(TIM4, &TIM_ICInitStr);

    TIM_ICInitStr.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStr.TIM_ICFilter = 0xf;
    TIM_ICInitStr.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM_ICInit(TIM4, &TIM_ICInitStr);

    // 配置TIM4为编码器模式，设置编码器模式为TI1和TI2模式，两相脉冲正向计数
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    // 使能TIM4
    TIM_Cmd(TIM4, ENABLE);
}


/**
  * @brief  初始化左侧编码器(重映射TIM3)
  */
void encoder_init_left() {
    // 使能GPIOB、TIM3和AFIO的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 配置引脚复用和SWJ-JTAG禁用
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
    // 配置GPIO端口
    GPIO_InitTypeDef GPIO_Init_encoderStr;
    GPIO_Init_encoderStr.GPIO_Mode = GPIO_Mode_IPU;  // 设置输入模式为上拉输入
    GPIO_Init_encoderStr.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;  // 配置引脚4和5
    GPIO_Init_encoderStr.GPIO_Speed = GPIO_Speed_50MHz;  // 设置端口速度
    GPIO_Init(GPIOB, &GPIO_Init_encoderStr);  // 初始化GPIO端口

    // 初始化TIM3的时基设置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInit_TIM3Str;
    TIM_TimeBaseInit_TIM3Str.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟分频
    TIM_TimeBaseInit_TIM3Str.TIM_RepetitionCounter = 0;  // 重复计数器
    TIM_TimeBaseInit_TIM3Str.TIM_CounterMode = TIM_CounterMode_Up;  // 计数器模式为向上计数
    TIM_TimeBaseInit_TIM3Str.TIM_Prescaler = 0;  // 预分频器
    TIM_TimeBaseInit_TIM3Str.TIM_Period = 65535;  // 计数器周期
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInit_TIM3Str);  // 初始化TIM3的时基设置

    // 初始化TIM3的编码器输入模式
    TIM_ICInitTypeDef TIM_ICInitStr;
    TIM_ICStructInit(&TIM_ICInitStr);
    TIM_ICInitStr.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStr.TIM_ICFilter = 0xf;
    TIM_ICInitStr.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInit(TIM3, &TIM_ICInitStr);

    TIM_ICInitStr.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStr.TIM_ICFilter = 0xf;
    TIM_ICInitStr.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM_ICInit(TIM3, &TIM_ICInitStr);

    // 配置TIM3为编码器模式，设置编码器模式为TI1和TI2模式，两相脉冲正向计数
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    // 使能TIM3
    TIM_Cmd(TIM3, ENABLE);
}


void encoder_cursor_init() {
    TIM_DeInit(TIM2);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_InitTypeDef GPIO_Init_encoderStr;
    GPIO_Init_encoderStr.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init_encoderStr.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init_encoderStr.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_Init_encoderStr);
    GPIO_Init_encoderStr.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOB, &GPIO_Init_encoderStr);


    TIM_TimeBaseInitTypeDef TIM_TimeBaseInit_TIM2Str;
    TIM_TimeBaseInit_TIM2Str.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit_TIM2Str.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit_TIM2Str.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit_TIM2Str.TIM_Prescaler = 0;
    TIM_TimeBaseInit_TIM2Str.TIM_Period = 2000;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInit_TIM2Str);

    TIM_ICInitTypeDef TIM_ICInitStr;
    TIM_ICStructInit(&TIM_ICInitStr);
    TIM_ICInitStr.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStr.TIM_ICFilter = 0xf;
    TIM_ICInitStr.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInit(TIM2, &TIM_ICInitStr);

    TIM_ICInitStr.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStr.TIM_ICFilter = 0xf;
    TIM_ICInitStr.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInit(TIM2, &TIM_ICInitStr);

    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_Cmd(TIM2, ENABLE);
}

void measure_speed_init() {
    encoder_init_left();
    encoder_init_right();
}

uint16_t encoder_get_right() {
    return TIM_GetCounter(TIM4);
}

uint16_t encoder_get_left() {
    return TIM_GetCounter(TIM3);
}

/// @brief 旋钮范围0-2000
uint16_t encoder_get_cursor() {
    return TIM_GetCounter(TIM2);
}

