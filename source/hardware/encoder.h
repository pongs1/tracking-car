#include "stm32f10x.h"

#ifndef __ENCODER_H
#define __ENCODER_H


void measure_speed_init();

void encoder_cursor_init();

uint16_t encoder_get_cursor();

uint16_t encoder_get_right();

uint16_t encoder_get_left();

#endif //__ENCODER_H
