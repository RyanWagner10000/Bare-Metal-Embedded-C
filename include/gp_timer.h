/*
 * file: gp_timer.h
 * description: header file for GP_TIMER peripheral structure
 * author: Ryan Wagner
 * date: November 22, 2025
 * notes:
 */

#ifndef GP_TIMER_H
#define GP_TIMER_H

#include <stdint.h>

#define TIM2_BASE 0x40000000

#define TIM2 ((GP_TIMER_TypeDef *)TIM2_BASE)

#define TIM2_EN (1U << 0)
#define TIM2_CR1_EN (1U << 0)
#define TIM2_SR_UIF (1U << 0)
#define TIM2_PSC 16000
#define TIM5_PSC 0
// #define TIM2_ARR 500
#define TIM2_ARR 10
#define TIM5_ARR 0

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
} GP_TIMER_TypeDef;

#endif // GP_TIMER_H
