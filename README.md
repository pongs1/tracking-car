# stm32循迹小车

本项目是基于STM32F103C8T6最小系统板的智能红外循迹小车，该小车利用红外传感器模块检测地面上的黑线信号，并通过位置环变积分PID控制算法精确控制左右轮子的转速，从而实现自动跟踪黑线行驶的功能。

## 硬件组成
主控制器：采用STM32F103C8T6最小系统板
电机驱动模块：使用L298N模块控制小车的左右轮电机
红外传感器模块：采用五路红外循迹模块，用于检测地面上的黑线信号
编码电机：用于提供小车的动力和轮子转速反馈
编码器：用于测量轮子的转速
OLED显示屏：用于展示PID参数信息和调节参数
按键：用于切换不同的参数和模式
## 工作原理
红外传感器模块通过发射红外光束并接收反射光信号，判断地面上的黑线位置。
主控制器获取红外传感器模块的数据，并将位置映射为20-80的范围，其中50为中间位置，20为偏左，80为偏右。
主控制器使用PID算法根据当前位置和目标位置计算出控制信号，精确调节左右轮子的转速。
电机驱动模块接收主控制器的控制信号，控制电机的转速和方向，使小车沿着黑线自动行驶。
## 软件控制
使用C语言编写控制程序，以下代码只展示主要逻辑、实现红外循迹算法、PID控制逻辑、电机控制逻辑、ui逻辑

source中的i2c和mpu6050在本项目中未使用原本想姿态解算的，但时间不足未有完成

在main.c文件中
   
    while (1) {  
        if (!run) { // 如果小车不在运行状态
            motor_init_flg = 0; // 重置电机初始化标志位
            mode_exchange(); // 切换模式
        } else {
            if (!motor_init_flg) { // 如果电机未初始化
                motor_re_init(); // 重新初始化电机
                TIM_DeInit(TIM2); // 关闭定时器2
                timer_init(); // 初始化定时器
                motor_init_flg = 1; // 设置电机初始化标志位
            }

            pid_turn(); // PID算法计算转向控制信号
            speed_ctrl_start(); // 开始速度控制
            OLED_ShowNum(1, 1, running_status, 3); // 在OLED显示屏上显示运行状态
            printf("%.1f,%.1f\n", encoder_angular_speed_L, encoder_angular_speed_R); // 打印左右轮子的角速度因为用vofa+调参达到数据稳定当去掉这行代码轮子转速反而不稳定
            key_status = Key_get(); // 获取按键状态
            if (key_status == 1) { // 如果按键状态为1（表示按下了停止按键）
                run = 0; // 将运行状态设置为0
                motor_deinit(); // 停止电机
                TIM_DeInit(TIM2); // 关闭定时器2
                encoder_cursor_init(); // 初始化编码器
                pid_speed_L_para.total_error = 0; // 重置PID积分
                pid_speed_R_para.total_error = 0;
                pid_infrared_para.total_error = 0;
            }
        }
    }
    
presentation.c文件用于构建简单的用户界面，通过OLED显示屏和按键实现PID参数的调节。用状态机来改进会更好

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
          case 1:
              mode_clear(&mode_clear_f);  // 清除模式标志位
              run = 0;  // 停止运行标志
              show_infrared_pid();  // 显示红外PID参数
              switch (cursor_site) {
                  case 1:
                      handle_mode1(base_speed, site.base_speed_line, site.base_speed_column);  // 处理模式1光标闪烁
                      base_speed = encoder_get_cursor() / 10;  // 设置基础速度
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


pid_speed.c文件实现了基于位置环变积分PID算法的精确控制左右轮子的转速。
    
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
    }
    void speed_set(PidTypeDef *PidSpeed_L_or_R, u16 rpm) {
      PidSpeed_L_or_R->target_val = rpm;
      PidSpeed_L_or_R->integral = PidSpeed_L_or_R->target_val / 300 * 2000;
    }

infrared_sensor.c文件包含红外传感器数据处理和映射逻辑，用于将红外传感器得到的位置映射到20-80的范围。
   
    void infrared_tell() {
        R_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
        MR_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
        M_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
        ML_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
        L_SEN = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
        if (Perfect_Normal_Running || Normal_Running) { running_status = 50; }//用宏定义
        else if (Slight_Left_Deviation) { running_status = 65; }
        else if (Slight_Right_Deviation) { running_status = 35; }
        else if (Severe_Left_Deviation) { running_status = 80; }
        else if (Severe_Right_Deviation) { running_status = 20; }
        else if (Crossing) { running_status = 50; }
        else if (Circle) { if (L_SEN == 0) { running_status = 30; } else { running_status = 70; }}
        else if (Stop) { running_status = 0; }
    }
# 视频结果演示


https://github.com/pongs1/tracking-car/assets/136407173/6d4a21f9-3d1f-4918-98d6-19622a534aca

