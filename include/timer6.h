/*
 * file: timer6.h
 * description: header file for timer 6 basic timer
 * author: Ryan Wagner
 * date: March 27, 2026
 * notes:
 */

#ifndef TIMER6_H
#define TIMER6_H

#include <stdint.h>
#include "rcc.h"

#define TIM6_BASE (0x40001000)
#define TIM6_EN (1U << 4)

#define TIM6 ((basic_timer_TypeDef *)TIM6_BASE)

typedef struct
{
    volatile uint32_t CR1;       // offset: 0x00
    volatile uint32_t CR2;       // offset: 0x04
    volatile uint32_t RESERVED1; // offset: 0x08
    volatile uint32_t DIER;      // offset: 0x0C
    volatile uint32_t SR;        // offset: 0x10
    volatile uint32_t EGR;       // offset: 0x14
    volatile uint32_t RESERVED2; // offset: 0x18
    volatile uint32_t RESERVED3; // offset: 0x1C
    volatile uint32_t RESERVED4; // offset: 0x20
    volatile uint32_t CNT;       // offset: 0x24
    volatile uint32_t PSC;       // offset: 0x28
    volatile uint32_t ARR;       // offset: 0x2C
} basic_timer_TypeDef;

void initTimer6(void);
void delayMicrosecond(uint32_t num_microseconds);

#endif // TIMER6_H
