//
// Created by 15297 on 2023/11/5.
//
#include "delay.h"

/**@brief 上拉输入GPIOA_PIN10、11(按键)*/
void KeyInit() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_key;
    GPIO_key.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_key.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10;
    GPIO_key.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_key);
}

/**按键检测GPIOA10、11当按下按键时keynum值为1、2*/
uint8_t Key_get() {
    uint8_t keynum = 0;
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == 0) {
        delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == 0)
            delay_ms(20);
        keynum = 1;


    }
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0) {
        delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0)
            delay_ms(20);
        keynum = 2;

    }
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0) {
        delay_ms(20);
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0) {
            delay_ms(20);
            keynum = 3;
        }
    }

    return keynum;
}


//uint8_t Key_get() {
//    static uint8_t keynum = 0;  // 按键状态，0表示未按下，1表示按下
//    static uint32_t debounceTime = 0;  // 按键消抖计时器
//
//    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0) {
//        if (debounceTime == 0) {
//            debounceTime = 20;  // 设置按键消抖时间，单位为毫秒
//            keynum = 1;
//        }
//    } else if (debounceTime > 0) {
//        debounceTime--;  // 减少消抖计时器的值
//    }
//
//    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == 0) {
//        if (debounceTime == 0) {
//            debounceTime = 20;  // 设置按键消抖时间，单位为毫秒
//            keynum = 2;
//        }
//    } else if (debounceTime > 0) {
//        debounceTime--;  // 减少消抖计时器的值
//    }
//
//    return keynum;
//}
