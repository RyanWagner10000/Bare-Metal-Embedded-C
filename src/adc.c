/*
 * file: adc.c
 * description: file that contains the helper functions for analog to digital convertors
 * author: Ryan Wagner
 * date: December 13, 2025
 * notes:
 */

#include "adc.h"

/**
 * @brief Initialize the Analog to Digital Converter on PA1
 *
 * @param None
 *
 * @return None
 *
 * @note Sets ADC to 8-bit accuracy
 */
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

/**
 * @brief Starts the ADC conversion on ADC1 PA1
 *
 * @param None
 *
 * @return None
 */
void startADCConversion(void)
{
    // Enable continuous conversion
    ADC1->CR2 |= CONT;

    // Start ADC conversion
    ADC1->CR2 |= SWSTART;

    return;
}

/**
 * @brief Read the value off the ADC1 Data Register if the status says it's ready
 *
 * @param None
 *
 * @return An unsigned 32 bit value
 */
uint32_t readADC(void)
{
    while (!(ADC1->SR & (1U << 1)))
        ;

    return ADC1->DR;
}
