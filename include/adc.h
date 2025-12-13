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

#define ADC_BASE 0x40004400

#define ADC2 ((ADC_TypeDef *)ADC2_BASE)

typedef struct
{
    volatile uint32_t SR;   // offset: 0x00
} ADC_TypeDef;

#endif // ADC_H
