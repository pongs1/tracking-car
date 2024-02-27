#include "stm32f10x.h"
#include "pid_speed.h"

#ifndef __PRESENTATION_H
#define __PRESENTATION_H
typedef struct {
    u16 kp, kd, ki;
} PidShowTypeDef;

static void init_mode_cursor(u8 *mode, u8 *cursor);

void site_init();

void pid_para_trans(PidShowTypeDef *pid_show, PidTypeDef *pid_para);

void mode_clear(u8 *mode_clear_f);


void maintain_show_val(const u8 *mode, const u8 *cursor_site);

void show_pwm();

static void sub_pwm_show(u16 pwm_L_or_R, u8 line, u8 column);

//static void handle_mode(u8 line, u8 column, const u8 *cursor, u8 arg_num);

static void handle_mode3(u16 pwm_L_or_R_value, u8 line, u8 column);

static void handle_mode3_R_Default();

static void handle_mode3_L_Default();

void show_L_speed_pid();

static void sub_pid_show(u16 pid_para_value, u8 line, u8 column, u8 is_ki);

static void handle_mode2(u16 pid_para_value, u8 line, u8 column, u8 is_ki);

static void handle_kp_Default(PidShowTypeDef pid_show);

static void handle_ki_Default(PidShowTypeDef pid_show);

static void handle_kd_Default(PidShowTypeDef pid_show);

void show_infrared_pid();

static void handle_mode1(u8 num, u8 line, u8 column);

#endif //__PRESENTATION_H
