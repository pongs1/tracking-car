#ifndef __SW_I2C_SOFTWARE_H
#define __SW_I2C_SOFTWARE_H
#define ACK_Y 1
#define ACK_N 0

#include "stm32f10x.h"

void SW_I2C_Init();

void SW_I2C_start();

void SW_I2C_end();

void SW_I2C_Send_Byte(u8 Byte);

u8 SW_I2C_Read_Byte();

void SW_I2C_Send_ACK(u8 ACK);

u8 SW_I2C_Receive_ACK();

#endif //__SW_I2C_SOFTWARE_H
