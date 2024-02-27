#include "stm32f10x.h"

/**
  * @note   时基单元向上计数，预分频为720,周期为1000,TIM1为100Hz
  * @note   TIM1为PWM1模式，PinA_0作为PWM输出引脚,输出低电平有效。
  */
void pwm_init(void) {
    // 启用GPIOA和TIM1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置GPIOA Pin 为复用推挽输出
    GPIO_InitTypeDef GPIO_TIM1_pwmStr;
    GPIO_TIM1_pwmStr.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_TIM1_pwmStr.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_TIM1_pwmStr.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_TIM1_pwmStr);

    // 配置TIM1的时基单元
    TIM_InternalClockConfig(TIM1);
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInitStructure.TIM_Period = 2000 - 1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    // 配置TIM1的输出比较通道1为PWM模式1
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;


    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);

    TIM_BDTRInitTypeDef TIM_BDTRStr;
    TIM_BDTRStructInit(&TIM_BDTRStr);
    TIM_BDTRConfig(TIM1, &TIM_BDTRStr);

    // 启用TIM1
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

}

//计数器
extern u16 num;

void TIM1_UP_IRQHandler() {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) {
        num++;
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

uint16_t pwm_left_value;///电机reinit时继承
uint16_t pwm_right_value;

/**
 * @brief 用定时器产生PWM，设置CCR
 *
 * 设置左侧电机的PWM值，并根据范围限制确保不超过最大值。
 * @param CCR PWM值，范围：0~2000
 * @note
 * - 此函数用于设置左侧电机的PWM值。
 * - 在设置之前，会检查PWM值是否超过最大值2000，如果超过则自动限制为2000。
 * - 使用定时器1（TIM1）的通道1（Channel 1）设置PWM值。
 * - 设置完成后，将设置的PWM值存储在全局变量pwm_left_value中。
 */
void PWM_SET_left(uint16_t CCR) {
    if (CCR > 2000) { CCR = 2000; }
    TIM_SetCompare1(TIM1, CCR);
    pwm_left_value = CCR;
//    if ((RCC->APB2ENR & (1 << 11)) != 0) { pwm_left_value = CCR; }
//    else {
//        pwm_init();
//        PWM_SET_left(CCR);
//    }

}

/**
 * @brief 用定时器产生PWM，设置CCR
 *
 * 设置右侧电机的PWM值，并根据范围限制确保不超过最大值。
 * @param CCR PWM值，范围：0~2000
 * @note
 * - 此函数用于设置右侧电机的PWM值。
 * - 在设置之前，会检查PWM值是否超过最大值2000，如果超过则自动限制为2000。
 * - 使用定时器1（TIM1）的通道2（Channel 2）设置PWM值。
 * - 设置完成后，将设置的PWM值存储在全局变量pwm_right_value中。
 */
void PWM_SET_right(uint16_t CCR) {
    if (CCR > 2000) { CCR = 2000; }
    TIM_SetCompare2(TIM1, CCR);
    pwm_right_value = CCR;
//    if ((RCC->APB2ENR & (1 << 11)) != 0) { pwm_right_value = CCR; }
//    else {
//        pwm_init();
//        PWM_SET_right(CCR);
//    }
}

/**
 * @brief 电机失能
 * @note 让io口初始化下载和复位时电机不一直转
 */
void motor_deinit() {
    TIM_DeInit(TIM1);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //让io口初始化下载和复位时电机不一直转
    GPIO_InitTypeDef GPIO_TIM1_pwmStr;
    GPIO_TIM1_pwmStr.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_TIM1_pwmStr.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_TIM1_pwmStr.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_TIM1_pwmStr);
}

/**
 * @brief 电机再使能
 */
void motor_re_init() {
    uint16_t savedCCRValue1;
    uint16_t savedCCRValue2;

    // 保存之前的 CCR 值
    savedCCRValue1 = pwm_left_value;
    savedCCRValue2 = pwm_right_value;
    pwm_init();
    TIM1->CCR1 = savedCCRValue1;
    TIM1->CCR2 = savedCCRValue2;
}

///重新命名函数
void motor_init() {
    pwm_init();
}