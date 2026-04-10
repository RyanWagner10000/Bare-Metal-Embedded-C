/*
 * file: timer8.h
 * description: header file for timer peripheral structure
 * author: Ryan Wagner
 * date: March 8, 2026
 * notes:
 */

#ifndef ADV_TIMER_H
#define ADV_TIMER_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"
#include "printing.h"

#define TIM8_BASE (0x40010400)

#define TIM8 ((ADV_TIMER_TypeDef *)TIM8_BASE)

#define TIM8_EN (1U << 1)
#define TIM8_MOTOR_PSC 15
// #define TIM8_MOTOR_ARR 19999 // 20ms period
#define TIM8_MOTOR_ARR 2499  // 2.5ms period
#define ESC_MIN_US 1100
#define ESC_RANGE_US 820

enum CHANNEL
{
    CH1,
    CH2,
    CH3,
    CH4
};

typedef struct
{
    volatile uint32_t CR1;   // offset: 0x00
    volatile uint32_t CR2;   // offset: 0x04
    volatile uint32_t SMCR;  // offset: 0x08
    volatile uint32_t DIER;  // offset: 0x0C
    volatile uint32_t SR;    // offset: 0x10
    volatile uint32_t EGR;   // offset: 0x14
    volatile uint32_t CCMR1; // offset: 0x18
    volatile uint32_t CCMR2; // offset: 0x1C
    volatile uint32_t CCER;  // offset: 0x20
    volatile uint32_t CNT;   // offset: 0x24
    volatile uint32_t PSC;   // offset: 0x28
    volatile uint32_t ARR;   // offset: 0x2C
    volatile uint32_t RCR;   // offset: 0x30
    volatile uint32_t CCR1;  // offset: 0x34
    volatile uint32_t CCR2;  // offset: 0x38
    volatile uint32_t CCR3;  // offset: 0x3C
    volatile uint32_t CCR4;  // offset: 0x40
    volatile uint32_t BDTR;  // offset: 0x44
    volatile uint32_t DCR;   // offset: 0x48
    volatile uint32_t DMAR;  // offset: 0x4C
} ADV_TIMER_TypeDef;

void initTimer8(void);
void setDuty(uint8_t channel, uint8_t duty);

#endif // ADV_TIMER_H
