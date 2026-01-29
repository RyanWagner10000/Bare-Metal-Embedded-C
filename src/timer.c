/*
 * file: timer.c
 * description: file that contains the helper functions for timers 2 & 5
 * author: Ryan Wagner
 * date: November 23, 2025
 * notes:
 */

#include "timer.h"

uint8_t check_imu_flag = 0;

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

uint8_t getImuFlag(void)
{
    return check_imu_flag;
}

void setImuFlag(uint8_t value)
{
    check_imu_flag = value;
    return;
}

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

void delay_ms(uint32_t num_milliseconds)
{
    for (uint32_t i = 0; i < num_milliseconds; i++)
    {
        TIM5->CNT = 0;
        while (TIM5->CNT < 99);
    }

    return;
}
