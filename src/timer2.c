/*
 * file: timer2.c
 * description: file that contains the helper functions for timers 2
 * author: Ryan Wagner
 * date: November 23, 2025
 * notes:
 */

#include "timer2_5.h"

uint8_t check_imu_flag = 0;

/**
 * @brief Gets the check_imu_flag global variable
 *
 * @param None
 *
 * @return None
 */
uint8_t getImuFlag(void)
{
    return check_imu_flag;
}

/**
 * @brief Sets the check_imu_flag global variable
 *
 * @param value Value to place in the check_imu_flag global variable
 *
 * @return None
 */
void setImuFlag(uint8_t value)
{
    check_imu_flag = value;
    return;
}

/**
 * @brief Initialize Timer2 peripheral to control update frequency for complimentary filter
 *
 * @param None
 *
 * @return None
 *
 * @note Enables a NVIC interrupt for when the timer overflows
 */
void initTimer2(void)
{
    // 250 Hz timer for 9-DOF IMU sensor
    // UEF = 16,000,000 / ((159 + 1) * (399 + 1)) = 250 Hz

    // Enable clock access to General Purpose Timer 2
    RCC->APB1ENR |= (1U << 0);

    // Enable update interupt
    TIM2->DIER |= (1U << 0);

    // Set Prescaler
    TIM2->PSC = 159;

    // Set Auto-reload Register
    TIM2->ARR = 399;

    // Set counter to 0
    TIM2->CNT = 0x00;

    // Enable ISER[0] bit 28, and set to priority 0 (highest)
    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn);

    // Enable TIM2 on Control Register AFTER setting everything up
    TIM2->CR1 = (1U << 0);

    return;
}
