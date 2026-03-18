/*
 * file: adv_timer.c
 * description: file that contains the helper functions for advanced timer 8
 * author: Ryan Wagner
 * date: March 8, 2026
 * notes:
 */

#include "timer8.h"

/**
 * @brief Initialize Timer8 peripheral to control PWM for motor control
 *
 * @param None
 *
 * @return None
 *
 * @note Uses pins PC6, PC7, PC8, and PC9
 */
void initTimer8(void)
{
    // 400 Hz timer for motor control/ESC's
    // UEF = 16,000,000 / ((159 + 1) * (249 + 1)) = 400 Hz
    // UEF = 16,000,000 / ((159 + 1) * (1999 + 1)) = 50 Hz
    
    // Enable clock access to Advanced Timer 8
    RCC->APB2ENR |= TIM8_EN;

    // Enable clock access to GPIOC
    RCC->AHB1ENR |= GPIOCEN;

    // Set alternate function mode to pins
    // GPIOC->MODER &= ~(255U << 12); // Clear
    GPIOC->MODER |= (170U << 12);  // Set

    // Set alternate function type AF3 for pins 6, 7, 8, 9
    // GPIOC->AFRL &= ~(255U << 24); // Clear
    GPIOC->AFRL |= (51U << 24);   // Set
    // GPIOC->AFRH &= ~(255U << 0);  // Clear
    GPIOC->AFRH |= (51U << 0);    // Set

    // Set Auto-reload preload
    TIM8->CR1 |= (1U << 7);

    // Set Compare Capture Mode for channels 1 & 2: PWM mode 1
    // TIM8->CCMR1 &= ~(7U << 12); // Clear
    TIM8->CCMR1 |= (6U << 12);  // Set
    TIM8->CCMR1 |= (1U << 11);  // Preload enable

    // TIM8->CCMR1 &= ~(7U << 4); // Clear
    TIM8->CCMR1 |= (6U << 4);  // Set
    TIM8->CCMR1 |= (1U << 3);  // Preload enable

    // Set Compare Capture Mode for channels 3 & 4: PWM mode 1
    // TIM8->CCMR2 &= ~(7U << 12); // Clear
    TIM8->CCMR2 |= (6U << 12);  // Set
    TIM8->CCMR2 |= (1U << 11);  // Preload enable

    // TIM8->CCMR2 &= ~(7U << 4); // Clear
    TIM8->CCMR2 |= (6U << 4);  // Set
    TIM8->CCMR2 |= (1U << 3);  // Preload enable

    // Enable all 4 channels output compare
    TIM8->CCER |= (1U << 0);
    TIM8->CCER |= (1U << 4);
    TIM8->CCER |= (1U << 8);
    TIM8->CCER |= (1U << 12);

    // Set counter to 0
    TIM8->CNT = 0;

    // Set Prescaler
    TIM8->PSC = TIM8_MOTOR_PSC;

    // Set Auto-reload Register
    TIM8->ARR = TIM8_MOTOR_ARR;

    // Set Capture Compare Register for channels 1-4
    TIM8->CCR1 = 1000;
    TIM8->CCR2 = 1000;
    TIM8->CCR3 = 1000;
    TIM8->CCR4 = 1000;

    // Set Event Generation
    TIM8->EGR |= (1U << 0);

    // Main Output Enable
    TIM8->BDTR |= (1U << 15);

    // Enable TIM8 on Control Register AFTER setting everything up
    TIM8->CR1 |= (1U << 0);

    return;
}

/**
 * @brief Set the duty cycle of a channel
 *
 * @param channel Which channel to update
 * @param duty Duty cycle value from 0-100
 *
 * @return None
 *
 * @note Uses pins PC6, PC7, PC8, and PC9
 */
void setDuty(uint8_t channel, uint8_t duty)
{
    // Make sure the input duty is valid
    if (duty > 100)
    {
        duty = 100;
    }

    // Calculate pulse microseconds for ESC
    uint32_t pulse_us = ESC_MIN_US + ((ESC_RANGE_US * duty) / 100);
    
    // Set duty on specific motor
    switch (channel)
    {
    case CH1:
    {
        TIM8->CCR1 = pulse_us;
        break;
    }
    case CH2:
    {
        TIM8->CCR2 = pulse_us;
        break;
    }
    case CH3:
    {
        TIM8->CCR3 = pulse_us;
        break;
    }
    case CH4:
    {
        TIM8->CCR4 = pulse_us;
        break;
    }
    default:
    {
        break;
    }
    }
    return;
}