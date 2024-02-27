
#include "dma_usart2.h"
#include "stm32f10x.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
/* 串口发送缓存区 */
#define USART2_MAX_SEND_LEN 1024                        /* 最大发送缓存字节数 */
//__align( 8 ) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN];     /* 发送缓冲,最大USART2_MAX_SEND_LEN字节 */
__attribute__((aligned(8))) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN];

/*
 * /USART2 DMA发送配置部分//
 * DMA1的各通道配置
 * 这里的传输形式是固定的,这点要根据不同的情况来修改
 * 从存储器->外设模式/8位数据宽度/存储器增量模式
 * DMA_CHx:DMA通道CHx
 * cpar:外设地址
 * cmar:存储器地址
 */
void UART_DMA_Config(DMA_Channel_TypeDef *DMA_CHx, u32 cpar, u32 cmar) {
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);                            /* 使能DMA传输 */
    DMA_DeInit(DMA_CHx);                                                          /* 将DMA的通道1寄存器重设为缺省值 */
    DMA_InitStructure.DMA_PeripheralBaseAddr = cpar;                         /* DMA外设ADC基地址 */
    DMA_InitStructure.DMA_MemoryBaseAddr = cmar;                         /* DMA内存基地址 */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;        /* 数据传输方向，从内存读取发送到外设 */
    DMA_InitStructure.DMA_BufferSize = 0;                            /* DMA通道的DMA缓存的大小 */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    /* 外设地址寄存器不变 */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;         /* 内存地址寄存器递增 */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  /* 数据宽度为8位 */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;      /* 数据宽度为8位 */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;              /* 工作在正常缓存模式 */
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;          /* DMA通道 x拥有中优先级 */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;              /* DMA通道x没有设置为内存到内存传输 */
    DMA_Init(DMA_CHx,
             &DMA_InitStructure);                                        /* 根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器 */
}


/* 开启一次DMA传输 */
void UART_DMA_Enable(DMA_Channel_TypeDef *DMA_CHx, u8 len) {
    DMA_Cmd(DMA_CHx, DISABLE);                                                    /* 关闭 指示的通道 */
    DMA_SetCurrDataCounter(DMA_CHx, len);                                         /* DMA通道的DMA缓存的大小 */
    DMA_Cmd(DMA_CHx, ENABLE);                                                     /* 开启DMA传输 */
}


/*
 * 初始化IO 串口2
 * pclk1:PCLK1时钟频率(Mhz)
 * bound:波特率
 */
void USART2_Init(u32 bound) {
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);                                 /* GPIOA时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    USART_DeInit(USART2);                                                                 /*复位串口2 */
    /* USART2_TX   PA.2 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                                           /* PA.2 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                      /*复用推挽输出 */
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                                /* 初始化PA2 */

    /* USART2_RX	  PA.3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                                /* 浮空输入 */
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                                /* 初始化PA3 */

    USART_InitStructure.USART_BaudRate = bound;                                /* 一般设置为9600; */
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                  /* 字长为8位数据格式 */
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                     /* 一个停止位 */
    USART_InitStructure.USART_Parity = USART_Parity_No;                      /* 无奇偶校验位 */
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;       /* 无硬件数据流控制 */
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;        /* 收发模式 */
    USART_Init(USART2, &USART_InitStructure);                                             /* 初始化串口	2 */

    /*
     * 波特率设置
     * USART2->BRR=(pclk1*1000000)/(bound);// 波特率设置
     * USART2->CR1|=0X200C;         //1位停止,无校验位.
     */
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);                                        /* 使能串口2的DMA发送 */
    UART_DMA_Config(DMA1_Channel7, (u32) &USART2->DR,
                    (u32) USART2_TX_BUF);              /* DMA1通道7,外设为串口2,存储器为USART2_TX_BUF */
    USART_Cmd(USART2, ENABLE);                                                            /* 使能串口 */


    /* 使能接收中断 */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                                        /* 开启中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;                            /* 抢占优先级3 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;                            /* 子优先级3 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                       /* IRQ通道使能 */
    NVIC_Init(&NVIC_InitStructure);                                                       /* 根据指定的参数初始化VIC寄存器 */
}


/*
 * 串口2,printf 函数
 * 确保一次发送数据不超过USART2_MAX_SEND_LEN字节
 */
void u2_printf(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *) USART2_TX_BUF, fmt, ap);
    va_end(ap);
    while (DMA_GetCurrDataCounter(DMA1_Channel7) !=
           0);                                                                       /* 等待通道7传输完成 */
    UART_DMA_Enable(DMA1_Channel7, strlen((const char *) USART2_TX_BUF));       /* 通过dma发送出去 */
}

void USART2_IRQHandler(void) {
    u8 res;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) /* 接收到数据 */
    {
        res = USART_ReceiveData(USART2);

    }
}