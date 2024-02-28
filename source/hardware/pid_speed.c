#include "pid_speed.h"
#include "stm32f10x.h"
#include "encoder.h"
#include "pwm_motor.h"
#include "infrared_sensor.h"

float encoder_rotations_L;
extern float encoder_angular_speed_L;
float encoder_rotations_R;
extern float encoder_angular_speed_R;
u16 base_speed = 75;

///在pid_speed.h里定义
PidTypeDef pid_speed_L_para = {3.00f, 1.40f, 0.111f};
PidTypeDef pid_speed_R_para = {3.00f, 1.40f, 0.112f};
PidTypeDef pid_infrared_para = {4.50f, 0.40f, 0};

/**
 * @brief 根据PID控制器的参数和当前的编码器角速度计算PID控制器的输出值。
 * @param PidSpeed_L_or_R 指向左或右电机的PID控制器结构体的指针
 * @param encoder_angular_speed 当前编码器的角速度，单位为rpm
 * @retval u16 PID控制器的输出值CCR
 *
 * @note
 * - 该函数计算PID控制器的输出值，用于控制电机或系统的运动。
 * - 它根据PID控制器的参数和当前的编码器角速度计算PID输出值，包括比例项、积分项、微分项等。
 * - 输出值经过范围限制，确保在合理范围内。
 * - 最后通过简单的缩放调整，将输出值调整到适当范围，以便控制电机或系统的运动。
 */
u16 pid_speed_cal(PidTypeDef *PidSpeed_L_or_R, float encoder_angular_speed) {
    u16 pid_output;  // 用于存储 PID 控制器的输出值，类型为无符号短整数（16位）

    // 计算角速度误差，目标角速度减去实际角速度
    PidSpeed_L_or_R->error = PidSpeed_L_or_R->target_val - encoder_angular_speed;

    // 积分项更新，累加速度误差
    PidSpeed_L_or_R->total_error += PidSpeed_L_or_R->error;
    if (PidSpeed_L_or_R->total_error > 2000) { PidSpeed_L_or_R->total_error = 2000; }
    if (PidSpeed_L_or_R->total_error < -2000) { PidSpeed_L_or_R->total_error = -2000; }
    // 计算PID控制器的各项分量
    PidSpeed_L_or_R->proportion = PidSpeed_L_or_R->kp * PidSpeed_L_or_R->error;
    PidSpeed_L_or_R->integral = PidSpeed_L_or_R->ki * PidSpeed_L_or_R->total_error;
    PidSpeed_L_or_R->differentiation = PidSpeed_L_or_R->kd * (PidSpeed_L_or_R->error - PidSpeed_L_or_R->last_error);

    // 对积分项和输出值进行范围限制
    PidSpeed_L_or_R->output =
            PidSpeed_L_or_R->proportion + PidSpeed_L_or_R->differentiation + PidSpeed_L_or_R->integral;
    if (PidSpeed_L_or_R->output > 300) { PidSpeed_L_or_R->output = 300; }

    // 计算PID控制器输出值，*2000/300 将转速rpm转换为CCR值
    pid_output = (u16) (PidSpeed_L_or_R->output * 20 / 3);

    return pid_output;

//    void pid_speed_cal() {
//        u16 pid_output;
//        static float ki_errors = 0;  // static 确保在函数调用之间保持持久性
//        float error_angular_speed;
//        float dt = /* 计算时间步长的代码 */;
//
//        target_angular_speed_L = (float) target_CCR_L / 10 * 3;
//        encoder_rotations_L = (float) encoder_get_left() / 2496;
//        error_angular_speed = target_angular_speed_L - encoder_angular_speed_L;
//
//        // 带有时间步长的积分项
//        ki_errors += error_angular_speed * dt;
//
//        // 比例和积分系数
//        pid_output = (u16)((pid_speed_L_para.kp * error_angular_speed + pid_speed_L_para.ki * ki_errors) * 5);
//
//        // 限制 PID 输出（可选）
//        // pid_output = (pid_output > MAX_OUTPUT) ? MAX_OUTPUT : pid_output;
//        // pid_output = (pid_output < MIN_OUTPUT) ? MIN_OUTPUT : pid_output;
//
//        // 设置左电机 PWM
//        PWM_SET_left(pid_output);
//    }

}


/**
 * @brief PID控制器初始化
 *
 * 初始化PID控制器，设置左右电机的PWM值以控制电机运动。
 * @note
 * - 调用pid_speed_cal函数计算左右电机的PWM值
 * - 之后调用speed_set来精准控速
 */
void speed_ctrl_start() {
    PWM_SET_left(pid_speed_cal(&pid_speed_L_para, encoder_angular_speed_L));
    PWM_SET_right(pid_speed_cal(&pid_speed_R_para, encoder_angular_speed_R));
}


/**
 * @brief 设置速度并初始化积分项
 * @param PidSpeed_L_or_R 指向左或右电机的PID控制器结构体的指针
 * @param rpm 目标速度，单位为每分钟转数（RPM）
 *
 * @note
 * - 它使用简化的方法初始化PID控制器的积分项，其中积分项设置为目标速度除以300后乘以2000。
 *   这是一种基本的初始化方法，可能需要根据系统的特定要求和特性进行调整。
 */
void speed_set(PidTypeDef *PidSpeed_L_or_R, u16 rpm) {
    PidSpeed_L_or_R->target_val = rpm;
    PidSpeed_L_or_R->integral = PidSpeed_L_or_R->target_val / 300 * 2000;
}

extern u8 running_status;

int16_t pid_turn_cal(PidTypeDef *PidInfrared) {
//    float linear_speed_L = (float) (encoder_angular_speed_L * 0.204203);
    int16_t pid_output;
    PidInfrared->target_val = 50;
    PidInfrared->error = PidInfrared->target_val - (float) running_status;
    PidInfrared->total_error += PidInfrared->error;

    PidInfrared->proportion = PidInfrared->kp * PidInfrared->error;
    PidInfrared->integral = PidInfrared->ki * PidInfrared->total_error;
    PidInfrared->differentiation = PidInfrared->kd * (PidInfrared->error - PidInfrared->last_error);

    PidInfrared->output =
            PidInfrared->proportion + PidInfrared->integral + PidInfrared->differentiation;
    pid_output = (int16_t) (PidInfrared->output);
    return pid_output;
}

extern int8_t stop;

void pid_turn() {
    int16_t pid_output = pid_turn_cal(&pid_infrared_para);
    int speed = base_speed;
    if (running_status == 0) {
        speed_set(&pid_speed_L_para, 0);
        speed_set(&pid_speed_R_para, 0);
        stop = 1;
    } else if (pid_output > -5 && pid_output < 5) {
        speed_set(&pid_speed_L_para, speed);
        speed_set(&pid_speed_R_para, speed);
    } else if (pid_output > 5) {

        speed = base_speed - pid_output;
        speed = (speed < 0) ? 0 : speed;
        speed_set(&pid_speed_L_para, speed);
        speed = base_speed + pid_output;
        speed = (speed > 255) ? 255 : speed;
        speed_set(&pid_speed_R_para, speed);

    } else if (pid_output < -5) {
        speed = base_speed + pid_output;
        speed = (speed < 0) ? 0 : speed;
        speed_set(&pid_speed_R_para, speed);
        speed = base_speed - pid_output;
        speed = (speed > 255) ? 255 : speed;
        speed_set(&pid_speed_L_para, speed);

    }
}