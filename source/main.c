#include "main.h"
#include "printf_uart.h"
#include <stdio.h>
#include <stm32f10x_usart.h>
#include "stm32f10x.h"
#include "arm_math.h"

/// @brief 共享变量
extern PidTypeDef pid_speed_L_para;
extern PidTypeDef pid_speed_R_para;
extern float encoder_angular_speed_L;
extern float encoder_angular_speed_R;
extern PidTypeDef pid_infrared_para;

extern u8 running_status;


///
/// @brief 标志位
static u8 motor_init_flg = 0;
u8 run = 0;
int key_status;


///
int main(void) {

    /// @brief 初始化
//    Uart2Init(9600); // 初始化Uart2
    USART2_Init(19200);  //串口2初始化
    PrintfInit(USART2); // printf 重定向到Uart1

    infrared_init();
    motor_deinit();         //防止电机上电一直转
    KeyInit();
    OLED_Init();            //与oled GPIO相通的这里为GPIOA初始化要放前面执行（原因未知）
    measure_speed_init();   //左右电机编码器初始化
    encoder_cursor_init();  //光标初始化

//    MPU_Init();
    ///



    while (1) {
        if (!run) {
            motor_init_flg = 0;
            mode_exchange();
        } else {

            if (!motor_init_flg) {
                motor_re_init();
                TIM_DeInit(TIM2);
                timer_init();
                motor_init_flg = 1;

            }

            pid_turn();
            speed_ctrl_start();
//            speed_set(&pid_speed_R_para, 10);
//            speed_set(&pid_speed_L_para, 10);
       
            OLED_ShowNum(1, 1, running_status, 3);
            printf("%.1f,%.1f\n", encoder_angular_speed_L, encoder_angular_speed_R);
            key_status = Key_get();
            if (key_status == 1) {
                run = 0;
                motor_deinit();
                TIM_DeInit(TIM2);
                encoder_cursor_init();
                pid_speed_L_para.total_error = 0;
                pid_speed_R_para.total_error = 0;
                pid_infrared_para.total_error = 0;
            }
        }
    }

}
