/*
 * file: adc.h
 * description: header file for ADC peripheral structure
 * author: Ryan Wagner
 * date: December 5, 2025
 * notes:
 */

#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"

#define ADC_BASE 0x40012000
#define ADC1_OFFSET 0x000
#define ADC2_OFFSET 0x100
#define ADC3_OFFSET 0x200
#define ADC_COMMON_OFFSET 0x300

#define ADC1EN (1U << 8)
#define ADCCH1 (1U << 0)
#define ADC_SEQ_LEN_1 0x00
#define ADCON (1U << 0)
#define CONT (1U << 1)
#define SWSTART (1U << 30)

#define ADC1 ((ADC_TypeDef *) (ADC_BASE+ADC1_OFFSET))
#define ADC2 ((ADC_TypeDef *) (ADC_BASE+ADC2_OFFSET))
#define ADC3 ((ADC_TypeDef *) (ADC_BASE+ADC3_OFFSET))
#define ADC_COMMON ((ADC_Common_TypeDef *)ADC_BASE+ADC_COMMON_OFFSET)

typedef struct
{
    volatile uint32_t SR;    // offset: 0x00
    volatile uint32_t CR1;   // offset: 0x04
    volatile uint32_t CR2;   // offset: 0x08
    volatile uint32_t SMPR1; // offset: 0x0C
    volatile uint32_t SMPR2; // offset: 0x10
    volatile uint32_t JOFR1; // offset: 0x14
    volatile uint32_t JOFR2; // offset: 0x18
    volatile uint32_t JOFR3; // offset: 0x1C
    volatile uint32_t JOFR4; // offset: 0x20
    volatile uint32_t HTR;   // offset: 0x24
    volatile uint32_t LTR;   // offset: 0x28
    volatile uint32_t SQR1;  // offset: 0x2C
    volatile uint32_t SQR2;  // offset: 0x30
    volatile uint32_t SQR3;  // offset: 0x34
    volatile uint32_t JSQR;  // offset: 0x38
    volatile uint32_t JDR1;  // offset: 0x3C
    volatile uint32_t JDR2;  // offset: 0x40
    volatile uint32_t JDR3;  // offset: 0x44
    volatile uint32_t JDR4;  // offset: 0x48
    volatile uint32_t DR;    // offset: 0x4C
} ADC_TypeDef;

typedef struct
{
    volatile uint32_t CSR;   // offset: 0x00
    volatile uint32_t CCR;   // offset: 0x04
    volatile uint32_t CDR;   // offset: 0x08
} ADC_Common_TypeDef;

void initADC(void);
void startADCConversion(void);
uint32_t readADC(void);

#endif // ADC_H
