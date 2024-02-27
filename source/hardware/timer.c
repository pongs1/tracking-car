
#include "stm32f10x.h"
#include "pid_speed.h"
#include "pwm_motor.h"
#include "encoder.h"
#include "infrared_sensor.h"


#define half_s 4999
#define a_second 9999
#define deci_second 999
#define half_deci_second 499
#define three_centi_second 299
#define centi_second 99

/**
 * @brief
 * @note 1. 启用TIM2外设的时钟。
 * @note 2. 配置TIM2以使用内部时钟。
 * @note 3. 设置定时器的时间基准参数。
 *    - 时钟分频：TIM_CKD_DIV1                                                    \n
 *    - 计数模式：TIM_CounterMode_Up                                              \n
 *    - 周期：10000 - 1（以实现1 Hz的中断频率）                                      \n
 *    - 预分频器：7200 - 1（根据需要调整以获取所需的时间基准）                           \n
 *    - 重复计数器：0
 * @note 4. 启用TIM2更新中断。】
 * @note 5. 配置TIM2_IRQn的NVIC，设置GROUP2,PR2,SUB1的中断优先级。
 * @note 6. 启用TIM2并启动定时器。
 */
void timer_init() {
    // 步骤1：启用TIM2外设的时钟。
    TIM_DeInit(TIM2);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 步骤2：配置TIM2以使用内部时钟。
    TIM_InternalClockConfig(TIM2);

    // 步骤3：设置定时器的时间基准参数。
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = three_centi_second;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    // 步骤4：启用TIM2更新中断。
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // 步骤5：配置TIM2_IRQn的NVIC，设置GROUP2,PR2,SUB1的中断优先级。
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    // 步骤6：启用TIM2并启动定时器。
    TIM_Cmd(TIM2, ENABLE);
}

/// @brief 时钟TIM_2中断函数
u16 num = 0;
extern float encoder_rotations_L;
extern PidTypeDef pid_speed_L_para;
float encoder_angular_speed_L;

extern float encoder_rotations_R;
extern PidTypeDef pid_speed_R_para;
float encoder_angular_speed_R;

u8 t1 = 75;

/**
 * @note 更新encoder_angular_speed
 * @note 计时
 * @note 更新pid_last_error
 */
void TIM2_IRQHandler() {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
        {
            // 获取左侧电机编码器的转速，通过编码器脉冲数/13*48*4得到转速值

            encoder_rotations_L = (float) encoder_get_left() / 2496;
            encoder_angular_speed_L = encoder_rotations_L * 25 * 60;

            encoder_rotations_R = (float) encoder_get_right() / 2496;
            encoder_angular_speed_R = encoder_rotations_R * 25 * 60;
            //
            infrared_tell();
            //
            num++;
            TIM_SetCounter(TIM3, 0);
            pid_speed_L_para.last_error = pid_speed_L_para.error;
            TIM_SetCounter(TIM4, 0);
            pid_speed_R_para.last_error = pid_speed_R_para.error;

        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}




/**速度测试(设置左轮pwm值)*/
//u8 t1 = 75;
//for (int i = 1; i <= 16; i++) {
//    if (num == i * t1) {
//        PWM_SET_left(2000 - (i - 1) * 100);
//        if (i == 16) {
//            num = 0;
//        }
//        break;
//    }
//}
