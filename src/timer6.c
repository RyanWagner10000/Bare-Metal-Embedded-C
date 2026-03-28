/*
 * file: timer6.c
 * description: file that contains the helper functions for basic timer 6
 * author: Ryan Wagner
 * date: March 27, 2026
 * notes:
 */

#include "timer6.h"

const uint16_t prescaler = 0;
const uint16_t autoreload = 16;

/**
 * @brief Initialize Timer6 peripheral for 1us delay
 *
 * @param None
 *
 * @return None
 */
void initTimer6(void)
{
    // 1,000,000 Hz timer for 1us waiting
    // UEF = 16,000,000 / ((0 + 1) * (16 + 1)) = 1,000,000 Hz

    // Enable clock access to General Purpose Timer 6
    RCC->APB1ENR |= TIM6_EN;

    // Set Prescaler
    TIM6->PSC = (uint32_t)prescaler;

    // Set Auto-reload Register
    TIM6->ARR = (uint32_t)autoreload;

    // Set counter to 0
    TIM6->CNT = 0;

    // Enable TIM2 on Control Register AFTER setting everything up
    TIM6->CR1 = (1U << 0);

    return;
}

/**
 * @brief Basic delay function in 1us increments
 *
 * @param num_microseconds Number of microseconds to delay
 *
 * @return None
 */
void delayMicrosecond(uint32_t num_microseconds)
{
    for (uint32_t i = 0; i < num_microseconds; i++)
    {
        TIM6->CNT = 0;
        while (TIM6->CNT < autoreload)
            ;
    }

    return;
}