#ifndef __DMA_USART2_H
#define __DMA_USART2_H

#include "stm32f10x.h"

void u2_printf(char *fmt, ...);

void USART2_Init(u32 bound);

#endif //__DMA_USART2_H
