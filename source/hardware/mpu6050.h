#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"

void MPU_Init();

void MPU_W_Reg_s(u8 Reg_Address, const u8 *Data);

void MPU_W_Reg(u8 Reg_Address, u8 Data);

u8 *MPU_R_Reg_s(u8 Reg_Address, u8 Byte_Num);

u8 MPU_R_Reg(u8 Reg_Address);

struct MPU_Data *MPU_Get();

struct MPU_Data {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t temp;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
};
#endif //__MPU6050_H
