#include <malloc.h>
#include "stm32f10x.h"
#include "I2C_software.h"
#include "mpu_reg.h"
#include "mpu6050.h"

#define MPU_W 0xD0
#define MPU_R 0xD1

#define data (const u8[])

void MPU_W_Reg_s(u8 Reg_Address, const u8 *Data) {
    SW_I2C_start();
    SW_I2C_Send_Byte(MPU_W);
    SW_I2C_Send_Byte(Reg_Address);
    for (int i = 0; i < sizeof Data; ++i) {
        SW_I2C_Send_Byte(Data[i]);
    }
    SW_I2C_end();
}

void MPU_W_Reg(u8 Reg_Address, u8 Data) {
    SW_I2C_start();
    SW_I2C_Send_Byte(MPU_W);
    SW_I2C_Send_Byte(Reg_Address);
    SW_I2C_Send_Byte(Data);
    SW_I2C_end();

}

u8 *MPU_R_Reg_s(u8 Reg_Address, u8 Byte_Num) {
    u8 *Data = (u8 *) malloc(sizeof(u8) * Byte_Num);
    if (Data == NULL) {
        return NULL;// 处理内存分配失败的情况
    }
    SW_I2C_start();
    SW_I2C_Send_Byte(MPU_W);
    SW_I2C_Send_Byte(Reg_Address);
    SW_I2C_start();
    SW_I2C_Send_Byte(MPU_R);
    for (int i = 0; i < Byte_Num - 1; ++i) {
        Data[i] = SW_I2C_Read_Byte();
        SW_I2C_Send_ACK(ACK_Y);
    }
    Data[Byte_Num - 1] = SW_I2C_Read_Byte();
    SW_I2C_Send_ACK(ACK_N);
    SW_I2C_end();
    return Data;
}

u8 MPU_R_Reg(u8 Reg_Address) {
    u8 Data;
    SW_I2C_start();
    SW_I2C_Send_Byte(MPU_W);
    SW_I2C_Send_Byte(Reg_Address);
    SW_I2C_start();
    SW_I2C_Send_Byte(MPU_R);
    Data = SW_I2C_Read_Byte();
    SW_I2C_Send_ACK(ACK_N);
    return Data;
}

void MPU_Init() {
    SW_I2C_Init();

    MPU_W_Reg(PWR_MGMT_1, 0x01);  //解除睡眠模式,设备不复位，不循环，温度传感器不失能，选择内部x陀螺仪时钟
    MPU_W_Reg(PWR_MGMT_2, 0x00);  //循环模式唤醒频率无，x、y、z轴不待机
    MPU_W_Reg(SMPLRT_DIV, 0x09);        //采样率分频寄存器，配置采样率
    MPU_W_Reg(CONFIG, 0x06);            //配置寄存器，配置DLPF
    MPU_W_Reg(GYRO_CONFIG, 0x10);    //陀螺仪配置寄存器，选择满量程为±2000°/s
    MPU_W_Reg(ACCEL_CONFIG, 0x18);    //加速度计配置寄存器，选择满量程为±16g
}


struct MPU_Data *MPU_Get() {
    static struct MPU_Data data1;
    u8 *Byte_Array = MPU_R_Reg_s(ACCEL_XOUT_H, 14); // 获取 u8 类型的数组
    int16_t Data[7]; // 由于 14 个 u8 合并成 7 个 int

    // 合并每两个 u8 元素为一个 int 元素
    for (int i = 0; i < 14; i += 2) {
        Data[i / 2] = (int16_t) ((u16) Byte_Array[i] << 8 | Byte_Array[i + 1]);
    }
    data1.accel_x = Data[0];
    data1.accel_y = Data[1];
    data1.accel_z = Data[2];
    data1.temp = Data[3];
    data1.gyro_x = Data[4];
    data1.gyro_y = Data[5];
    data1.gyro_z = Data[6];
    free(Byte_Array);
    return &data1;
}
