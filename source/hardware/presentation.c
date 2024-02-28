#include "OLED.h"
#include "stm32f10x.h"
#include "key.h"
#include "presentation.h"
#include "delay.h"
#include "encoder.h"
#include "pwm_motor.h"
#include "pid_speed.h"

#define Yes 1
#define NO 0
static u8 count = 0;//闪烁
struct {
    u8 pwm_L_line,
            pwm_L_column,
            pwm_R_line,
            pwm_R_column;
    u8 kp_line,
            ki_line,
            kd_line,
            kp_column,
            ki_column,
            kd_column,
            base_speed_line,
            base_speed_column;
} site;

PidShowTypeDef pid_show_speed_L;
PidShowTypeDef pid_show_infrared;
struct {
    u16 pwm_L, pwm_R;
} pwm_show;


extern PidTypeDef pid_speed_L_para;
extern PidTypeDef pid_infrared_para;
extern u8 run;
extern u16 base_speed;

void mode_exchange() {

    static u8 mode;  // 保持 mode 在函数调用之间的状态
    static u8 cursor_site;  // 保持 cursor_site 在函数调用之间的状态
    static u8 initialized = 0;  // 静态标志，标记是否已经初始化过
    static u8 mode_clear_f = 0; //清屏标志
    int key_status;

    key_status = Key_get();  // 获取按键状态

    if (!initialized) {
        // 在第一次调用时初始化 mode = 1 和 cursor_site = 0
        init_mode_cursor(&mode, &cursor_site);
        initialized = 1;
    }

    site_init();  // 初始化站点
    pid_para_trans(&pid_show_speed_L, &pid_speed_L_para);  // 转换左轮速度PID参数
    pid_para_trans(&pid_show_infrared, &pid_infrared_para);  // 转换红外PID参数

    switch (mode) {
///设置pwm的模式
//        case 3:
//            mode_clear(&mode_clear_f);
//            run = 0;
//            show_pwm();
//            switch (cursor_site) {
//                case 1:
//                    handle_mode3_L_Default();
//                    pwm_show.pwm_L = encoder_get_cursor();
//                    OLED_ShowNum(3, 1, encoder_get_cursor(), 6);
//                    break;
//                case 2:
//                    handle_mode3_R_Default();
//                    pwm_show.pwm_R = encoder_get_cursor();
//                    OLED_ShowNum(3, 1, encoder_get_cursor(), 6);
//                    break;
//                default:
//                    cursor_site = 0;
//            }
//            break;
        case 1:
            mode_clear(&mode_clear_f);  // 清除模式标志位
            run = 0;  // 停止运行标志
            show_infrared_pid();  // 显示红外PID参数
            switch (cursor_site) {
                case 1:
                    handle_mode1(base_speed, site.base_speed_line, site.base_speed_column);  // 处理模式1光标闪烁
                    base_speed = encoder_get_cursor() / 8;  // 设置基础速度
                    break;
                case 2:
                    handle_kp_Default(pid_show_infrared);  // 处理红外PID的kp参数
                    pid_infrared_para.kp = (float) (encoder_get_cursor() / 100.0);  // 设置红外PID的kp参数
                    break;
                case 3:
                    handle_kd_Default(pid_show_infrared);  // 处理红外PID的kd参数
                    pid_infrared_para.kd = (float) (encoder_get_cursor() / 100.0);  // 设置红外PID的kd参数
                    break;
                case 4:
                    handle_ki_Default(pid_show_infrared);  // 处理红外PID的ki参数
                    pid_infrared_para.ki = (float) (encoder_get_cursor() / 1000.0);  // 设置红外PID的ki参数
                    break;
                default:
                    cursor_site = 0;  // 光标位置复位
            }
            break;
        case 2:
            mode_clear(&mode_clear_f);  // 清除模式标志位
            run = 0;  // 停止运行
            show_L_speed_pid();  // 显示左轮速度PID参数
            switch (cursor_site) {
                case 1:
                    handle_kp_Default(pid_show_speed_L);  // 处理左轮速度PID的kp参数
                    pid_speed_L_para.kp = (float) (encoder_get_cursor() / 100.0);  // 设置左轮速度PID的kp参数
                    break;
                case 2:
                    handle_kd_Default(pid_show_speed_L);  // 处理左轮速度PID的kd参数
                    pid_speed_L_para.kd = (float) (encoder_get_cursor() / 100.0);  // 设置左轮速度PID的kd参数
                    break;
                case 3:
                    handle_ki_Default(pid_show_speed_L);  // 处理左轮速度PID的ki参数
                    pid_speed_L_para.ki = (float) (encoder_get_cursor() / 1000.0);  // 设置左轮速度PID的ki参数
                    break;
                default:
                    cursor_site = 0;  // 光标位置复位
            }
            break;

        default:
            PWM_SET_left(pwm_show.pwm_L);  // 设置左轮PWM
            PWM_SET_right(pwm_show.pwm_R);  // 设置右轮PWM
            mode_clear(&mode_clear_f);  // 清除模式标志位
            OLED_ShowString(4, 5, "RUN");  // 在OLED上显示"RUN"
            mode = 1;  // 模式设为1
            run = 1;  // 开始运行
            mode_clear_f = 0;  // 清除模式标志位
    }

    if (key_status == 1) {
        mode += 1;  // 模式加1
        mode_clear_f = 0;  // 清除模式标志位
        cursor_site = 0;  // 光标位置复位
    }
    if (key_status == 2) {
        cursor_site += 1;  // 光标位置加1
        count = 0;  // 计数清零
        maintain_show_val(&mode, &cursor_site);  // 继承显示数值方便后续改进参数
    }
}


///@brief 模式和光标初始化
void init_mode_cursor(u8 *mode, u8 *cursor_site) {
    *mode = 1;
    *cursor_site = 0;
}

void site_init() {
    site.pwm_L_line = 1;
    site.pwm_L_column = 6;
    site.pwm_R_line = 2;
    site.pwm_R_column = 6;
    site.kp_line = 2;
    site.kp_column = 4;
    site.kd_line = 3;
    site.kd_column = 4;
    site.ki_line = 4;
    site.ki_column = 4;
    site.base_speed_line = 1;
    site.base_speed_column = 6;

}

void pid_para_trans(PidShowTypeDef *pid_show, PidTypeDef *pid_para) {
    pid_show->kp = (u16) (pid_para->kp * 100.0);
    pid_show->kd = (u16) (pid_para->kd * 100.0);
    pid_show->ki = (u16) (pid_para->ki * 1000.0);
}

///换模式时清屏
void mode_clear(u8 *mode_clear_f) {
    if (!*mode_clear_f) {
        OLED_Clear();
        *mode_clear_f = 1;
    }
}


void maintain_show_val(const u8 *mode, const u8 *cursor_site) {
    PidShowTypeDef *pid_show;
    switch (*mode) {
        case 1:
            pid_show = &pid_show_infrared;
            break;
        case 2:
            pid_show = &pid_show_speed_L;
            break;
//        case 3:
//            switch (*cursor_site) {
//                case 1:
//                    TIM2->CNT = pwm_show.pwm_L;
//                    break;
//                case 2:
//                    TIM2->CNT = pwm_show.pwm_R;
//                    break;
//            }
//            break;
        default:
            return;
    }

    switch (*cursor_site) {
        case 1:
            TIM2->CNT = base_speed * 8;
            break;
        case 2:
            TIM2->CNT = pid_show->kp;
            break;
        case 3:
            TIM2->CNT = pid_show->kd;
            break;
        case 4:
            TIM2->CNT = pid_show->ki;
            break;
        default:;
    }
}


///@brief OLED显示pwm(已配置为Default)
void show_pwm() {
    OLED_ShowString(site.pwm_L_line, site.pwm_L_column - 5, "PWM:");
    sub_pwm_show(pwm_show.pwm_L, site.pwm_L_line, site.pwm_L_column);
    sub_pwm_show(pwm_show.pwm_R, site.pwm_R_line, site.pwm_R_column);
//    sub_pwm_show(pwm_left_value, 1, 5);
//    sub_pwm_show(pwm_right_value, 1, 11);
}

/**
 * @brief 在OLED屏幕上显示___._%格式的PWM值。
 * @param pwm_L_or_R_value 的PWM值(传入三位数整数)
 * @param line 显示的行
 * @param column 显示的列
 */
static void sub_pwm_show(u16 pwm_L_or_R_value, u8 line, u8 column) {
    uint8_t remain;
    remain = pwm_L_or_R_value % 20;    // 提取PWM值的最后一位数字
    pwm_L_or_R_value /= 20;    // 从PWM值中去掉最后一位数字
    OLED_ShowNum(line, column, pwm_L_or_R_value, 3);    // 显示PWM值的百位和十位和个位数字
    OLED_ShowString(line, column + 3, ".");    // 显示小数点
    OLED_ShowNum(line, column + 4, remain, 1);    // 显示PWM小数部分
    OLED_ShowString(line, column + 5, "%");    // 显示百分比符号
}

/**
 * @brief 基于sub_pwm_show的光标(闪烁)显示
 * @param pwm_L_or_R_value PWM的左右标识
 * @param line OLED显示的行
 * @param column OLED显示的列
 * @note 此函数依赖外部定义的计数器u8 count。
 * @note 只闪烁2次防止delay阻塞程序
 */
static void handle_mode3(u16 pwm_L_or_R_value, u8 line, u8 column) {
    if (count > 1) {
        sub_pwm_show(pwm_L_or_R_value, line, column);
    } else {
        sub_pwm_show(pwm_L_or_R_value, line, column);
        delay_ms(50);
        OLED_ShowString(line, column, "     ");
        delay_ms(125);
        count++;
    }
}

///@brief 默认首行显示
static void handle_mode3_L_Default() {
    handle_mode3(pwm_show.pwm_L, site.pwm_L_line, site.pwm_L_column);
}

///@brief 默认首行显示
static void handle_mode3_R_Default() {
    handle_mode3(pwm_show.pwm_R, site.pwm_R_line, site.pwm_R_column);
}

///@brief 参照show_pwm
void show_L_speed_pid() {
    OLED_ShowString(1, 1, "L_speed_pid");

    OLED_ShowString(site.kp_line, site.kp_column - 3, "kp:");
    sub_pid_show(pid_show_speed_L.kp, site.kp_line, site.kp_column, NO);

    OLED_ShowString(site.kd_line, site.kd_column - 3, "kd:");
    sub_pid_show(pid_show_speed_L.kd, site.kd_line, site.kd_column, NO);

    OLED_ShowString(site.ki_line, site.ki_column - 3, "ki:");
    sub_pid_show(pid_show_speed_L.ki, site.ki_line, site.ki_column, Yes);
}

/**
 * @brief 参照sub_pwm_show
 * @note 区别了kp、kd和ki两种显示格式
 */
static void sub_pid_show(u16 pid_show_value, u8 line, u8 column, u8 is_ki) {
    uint8_t percentile, decile;
    if (is_ki) {
        uint8_t millennial;
        millennial = pid_show_value % 10;    // 提取pid值的最后一位数字
        pid_show_value /= 10;    // 从pid值中去掉最后一位数字
        percentile = pid_show_value % 10;    // 提取pid值的倒数第2位数字
        pid_show_value /= 10;    // 从pid值中去掉最后2位数字
        decile = pid_show_value % 10;
        pid_show_value /= 10;

        OLED_ShowNum(line, column, pid_show_value, 2);    // 显示pid值的十位和个位数字
        OLED_ShowString(line, column + 2, ".");    // 显示小数点
        OLED_ShowNum(line, column + 3, decile, 1);    // 显示pid小数十分位部分
        OLED_ShowNum(line, column + 4, percentile, 1);    // 显示pid小数百分位部分
        OLED_ShowNum(line, column + 5, millennial, 1);
    } else {
        percentile = pid_show_value % 10;    // 提取pid值的最后一位数字
        pid_show_value /= 10;    // 从pid值中去掉最后一位数字
        decile = pid_show_value % 10;    // 提取pid值的倒数第2位数字
        pid_show_value /= 10;    // 从pid值中去掉最后2位数字
        OLED_ShowNum(line, column, pid_show_value, 3);    // 显示pid值的百位和十位和个位数字
        OLED_ShowString(line, column + 3, ".");    // 显示小数点
        OLED_ShowNum(line, column + 4, decile, 1);    // 显示pid小数十分位部分
        OLED_ShowNum(line, column + 5, percentile, 1);    // 显示pid小数百分位部分
    }
}

/**
 * @brief 参照handle_mode3
 */
static void handle_mode2(u16 pid_para_value, u8 line, u8 column, u8 is_ki) {
    if (count > 1) {
        sub_pid_show(pid_para_value, line, column, is_ki);
    } else {
        sub_pid_show(pid_para_value, line, column, is_ki);
        delay_ms(50);
        OLED_ShowString(line, column, "      ");
        delay_ms(125);
        count++;
    }
}

static void handle_kp_Default(PidShowTypeDef pid_show) {
    handle_mode2(pid_show.kp, site.kp_line, site.kp_column, NO);
}

static void handle_ki_Default(PidShowTypeDef pid_show) {
    handle_mode2(pid_show.ki, site.ki_line, site.ki_column, Yes);
}

static void handle_kd_Default(PidShowTypeDef pid_show) {
    handle_mode2(pid_show.kd, site.kd_line, site.kd_column, NO);
}


void show_infrared_pid() {
//    OLED_ShowString(1, 1, "infrared_pid");
    OLED_ShowString(site.base_speed_line, site.base_speed_column - 5, "base:");
    OLED_ShowNum(site.base_speed_line, site.base_speed_column, base_speed, 3);

    OLED_ShowString(site.kp_line, site.kp_column - 3, "kp:");
    sub_pid_show(pid_show_infrared.kp, site.kp_line, site.kp_column, NO);

    OLED_ShowString(site.kd_line, site.kd_column - 3, "kd:");
    sub_pid_show(pid_show_infrared.kd, site.kd_line, site.kd_column, NO);

    OLED_ShowString(site.ki_line, site.ki_column - 3, "ki:");
    sub_pid_show(pid_show_infrared.ki, site.ki_line, site.ki_column, Yes);
}

static void handle_mode1(u8 num, u8 line, u8 column) {
    if (count > 1) {
        OLED_ShowNum(line, column, num, 3);
    } else {
        OLED_ShowNum(line, column, num, 3);
        delay_ms(50);
        OLED_ShowString(line, column, "   ");
        delay_ms(125);
        count++;
    }
}

/**case处理失败(无法在变换模式后使用旧参，同时变换模式前更新旧参)*/
//static void handle_mode(u8 line, u8 column, const u8 *cursor, u8 arg_num) {
//    static u8 last_line, last_column, have_last = 0, counter = 0;
//
//    if (have_last)OLED_ShowChar(last_line, last_column, ' ');
//    OLED_ShowChar(line, column - 1, '>');
//    last_line = line;
//    last_column = column - 1;
//    if (counter != *cursor && counter != 0) {
//        counter = *cursor;
//    }
//    if (counter > 1) {
//        have_last = 1;
//    }
//    if (counter == arg_num) {
//        have_last = 0;
//    }
//}
