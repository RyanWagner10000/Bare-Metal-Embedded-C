/*
 * file: timer5.c
 * description: file that contains the helper functions for timers 5
 * author: Ryan Wagner
 * date: November 23, 2025
 * notes:
 */

#include "timer2_5.h"

/**
 * @brief Initialize Timer5 peripheral for 1ms delay
 *
 * @param None
 *
 * @return None
 */
void initTimer5(void)
{
    // 1000 Hz timer for 1ms waiting
    // UEF = 16,000,000 / ((159 + 1) * (99 + 1)) = 1000 Hz

    // Enable clock access to General Purpose Timer 5
    RCC->APB1ENR |= (1U << 3);

    // Set Prescaler
    TIM5->PSC = 159;

    // Set Auto-reload Register
    TIM5->ARR = 99;

    // Set counter to 0
    TIM5->CNT = 0;

    // Enable TIM2 on Control Register AFTER setting everything up
    TIM5->CR1 = (1U << 0);

    return;
}

/**
 * @brief Basic delay function in 1ms increments
 *
 * @param num_milliseconds Number of milliseconds to delay
 *
 * @return None
 */
void delayMillisecond(uint32_t num_milliseconds)
{
    for (uint32_t i = 0; i < num_milliseconds; i++)
    {
        TIM5->CNT = 0;
        while (TIM5->CNT < 99)
            ;
    }

    return;
}
