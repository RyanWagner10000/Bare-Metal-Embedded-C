/*
 * file: adc.c
 * description: file that contains the helper functions for analog to digital convertors
 * author: Ryan Wagner
 * date: December 13, 2025
 * notes:
 */

#include "adc.h"

void initADC(void)
{
    // Enable clock to GPIOA
    RCC->AHB1ENR |= GPIOAEN;

    // Set PA1 to Analog mode
    GPIOA->MODER |= (1U << 2);
    GPIOA->MODER |= (1U << 3);

    // Enable clock access to ADC
    RCC->APB2ENR |= ADC1EN;

    // Set conversion sequence start
    ADC1->SQR3 = ADCCH1;

    // Set sequence length
    ADC1->SQR1 = ADC_SEQ_LEN_1;

    // Set data bits to 8
    ADC1->CR1 |= (1U << 25);
    ADC1->CR1 &= ~(1U << 24);

    // Enable ADC module
    ADC1->CR2 |= ADCON;

    return;
}

void startADCConversion(void)
{
    // Enable continuous conversion
    ADC1->CR2 |= CONT;

    // Start ADC conversion
    ADC1->CR2 |= SWSTART;

    return;
}

uint32_t readADC(void)
{
    while (!(ADC1->SR & (1U << 1)))
        ;

    return ADC1->DR;
}