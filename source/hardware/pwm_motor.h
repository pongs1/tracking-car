#ifndef __PWM_H
#define __PWM_H

void pwm_init();

void PWM_SET_left(uint16_t CCR);

void PWM_SET_right(uint16_t CCR);

void motor_init();

void motor_re_init();

void motor_deinit();

#endif //__PWM_H
