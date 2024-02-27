#ifndef __PID_SPEED_H
#define __PID_SPEED_H

#include "stm32f10x.h"

#define target_angular_speed_L target_val
#define target_angular_speed_R target_val
typedef struct {
    float kp, kd, ki;
    float error, last_error, total_error;
    float proportion, differentiation,
            integral, max_integral;
    float output;
    float target_val;
} PidTypeDef;


u16 pid_speed_cal(PidTypeDef *PidSpeed_L_or_R, float encoder_angular_speed);

void speed_set(PidTypeDef *PidSpeed_L_or_R, u16 rpm);

void speed_ctrl_start();

int16_t pid_turn_cal(PidTypeDef *PidInfrared);

void pid_turn();

#endif //__PID_SPEED_H
