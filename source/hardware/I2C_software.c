#include "stm32f10x.h"
#include "delay.h"
#include "I2C_software.h"

#define SCL_PORT GPIOB
#define SDA_PORT GPIOB
#define SCL_PIN GPIO_Pin_0
#define SDA_PIN GPIO_Pin_1


void SW_I2C_Init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef SW_SW_I2C_InitStr;
    SW_SW_I2C_InitStr.GPIO_Pin = SCL_PIN;
    SW_SW_I2C_InitStr.GPIO_Mode = GPIO_Mode_Out_OD;
    SW_SW_I2C_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SCL_PORT, &SW_SW_I2C_InitStr);
    SW_SW_I2C_InitStr.GPIO_Pin = SDA_PIN;
    GPIO_Init(SDA_PORT, &SW_SW_I2C_InitStr);

    GPIO_SetBits(SCL_PORT, SCL_PIN);
    GPIO_SetBits(SDA_PORT, SDA_PIN);
}

void SCL_W_H() {
    GPIO_SetBits(SCL_PORT, SCL_PIN);
    delay_us(2);
}

void SDA_W_H() {
    GPIO_SetBits(SDA_PORT, SDA_PIN);
    delay_us(2);
}

void SCL_W_L() {
    GPIO_ResetBits(SCL_PORT, SCL_PIN);
    delay_us(2);
}

void SDA_W_L() {
    GPIO_ResetBits(SDA_PORT, SDA_PIN);
    delay_us(2);
}

u8 SDA_R() {
    u8 bit_value = GPIO_ReadInputDataBit(SDA_PORT, SDA_PIN);
    delay_us(2);
    return bit_value;
}

void SW_I2C_start() {
    SDA_W_H();
    SCL_W_H();
    SDA_W_L();
    SCL_W_L();
}

void SW_I2C_end() {
    SDA_W_L();
    SCL_W_H();
    SDA_W_H();
}

void SW_I2C_Send_Byte(u8 Byte) {
    for (int i = 0; i < 8; ++i) {
        if ((Byte & (0x80 >> i)) != 0) {
            SDA_W_H();
            SCL_W_H();
            SCL_W_L();
        } else {
            SDA_W_L();
            SCL_W_H();
            SCL_W_L();
        }
    }
    SW_I2C_Receive_ACK();
}

u8 SW_I2C_Read_Byte() {
    u8 Byte = 0x00;
    SDA_W_H();
    for (int i = 0; i < 8; ++i) {
        SCL_W_H();
        if (SDA_R() == 1) {
            Byte |= 0x80 >> i;
        }
        SCL_W_L();
    }
    return Byte;
}

void SW_I2C_Send_ACK(u8 ACK) {
    if (ACK) { SDA_W_L(); } else { SDA_W_H(); }
    SCL_W_H();
    SCL_W_L();
}

u8 SW_I2C_Receive_ACK() {
    u8 ACK;
    SDA_W_H();
    SCL_W_H();
    ACK = SDA_R();
    SCL_W_L();
    return ACK;
}
