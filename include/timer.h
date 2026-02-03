/*
 * file: timer.h
 * description: header file for timer peripheral structure
 * author: Ryan Wagner
 * date: November 22, 2025
 * notes:
 */

#ifndef timer_H
#define timer_H

#include <stdint.h>
#include "rcc.h"
#include "nvic.h"

#define TIM2_BASE (0x40000000)
#define TIM5_BASE (0x40000C00)

#define TIM2 ((timer_TypeDef *)TIM2_BASE)
#define TIM5 ((timer_TypeDef *)TIM5_BASE)

#define TIM_SR_UIF (1U << 0)

typedef struct
{
    volatile uint32_t CR1;       // offset: 0x00
    volatile uint32_t CR2;       // offset: 0x04
    volatile uint32_t SMCR;      // offset: 0x08
    volatile uint32_t DIER;      // offset: 0x0C
    volatile uint32_t SR;        // offset: 0x10
    volatile uint32_t EGR;       // offset: 0x14
    volatile uint32_t CCMR1;     // offset: 0x18
    volatile uint32_t CCMR2;     // offset: 0x1C
    volatile uint32_t CCER;      // offset: 0x20
    volatile uint32_t CNT;       // offset: 0x24
    volatile uint32_t PSC;       // offset: 0x28
    volatile uint32_t ARR;       // offset: 0x2C
    volatile uint32_t RESERVED1; // offset: 0x30
    volatile uint32_t CCR1;      // offset: 0x34
    volatile uint32_t CCR2;      // offset: 0x38
    volatile uint32_t CCR3;      // offset: 0x3C
    volatile uint32_t CCR4;      // offset: 0x40
    volatile uint32_t RESERVED2; // offset: 0x44
    volatile uint32_t DCR;       // offset: 0x48
    volatile uint32_t DMAR;      // offset: 0x4C
    volatile uint32_t OR;        // offset: 0x50
} timer_TypeDef;

void setImuFlag(uint8_t value);
uint8_t getImuFlag(void);
void initTimer2(void);
void initTimer5(void);
void delay_ms(uint32_t num_milliseconds);

#endif // timer_H
