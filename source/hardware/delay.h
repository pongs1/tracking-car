#ifndef __DELAY_H_
#define __DELAY_H_

#include "stm32f10x.h"

void delay_ms(uint32_t xms);

void delay_us(uint32_t xus);

void delay_s(uint32_t xs);

void SysTick_Init();

#endif