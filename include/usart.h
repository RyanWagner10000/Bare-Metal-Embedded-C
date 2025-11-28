/*
 * file: usart.h
 * description: header file for USART peripheral structure
 * author: Ryan Wagner
 * date: November 22, 2025
 * notes:
 */

#ifndef USART_H
#define USART_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"

#define USART2_BASE 0x40004400

#define USART2 ((USART_TypeDef *)USART2_BASE)

#define GPIODEN (1U << 3)
#define USART2EN (1U << 17)

#define BAUDRATE 9600
#define CLOCK_FREQ 16000000
#define CR1_TE (1U << 3)
#define CR1_UE (1U << 13)
#define SR_TXE (1U << 7)

typedef struct
{
    volatile uint32_t SR;  // offset: 0x00
    volatile uint32_t DR;  // offset: 0x04
    volatile uint32_t BRR; // offset: 0x08
    volatile uint32_t CR1; // offset: 0x0C
    volatile uint32_t CR2; // offset: 0x10
    volatile uint32_t CR3; // offset: 0x14
    volatile uint32_t CR4; // offset: 0x18
} USART_TypeDef;

void initUSART(void);

#endif // USART_H
