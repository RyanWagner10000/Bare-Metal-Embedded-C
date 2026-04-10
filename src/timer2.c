/*
 * file: timer2.c
 * description: file that contains the helper functions for timers 2
 * author: Ryan Wagner
 * date: November 23, 2025
 * notes:
 */

#include "timer2_5.h"

volatile uint8_t flag_imu_update = 0;
volatile uint8_t flag_pwm_update = 0;
volatile uint8_t flag_one_second = 0;
volatile uint32_t tick_count;

/**
 * @brief Gets the flag_imu_update global variable
 *
 * @param None
 *
 * @return None
 */
uint8_t getImuFlag(void)
{
    return flag_imu_update;
}

/**
 * @brief Sets the flag_imu_update global variable
 *
 * @param value Value to place in the flag_imu_update global variable
 *
 * @return None
 */
void setImuFlag(uint8_t value)
{
    flag_imu_update = value;
    return;
}

/**
 * @brief Gets the flag_pwm_update global variable
 *
 * @param None
 *
 * @return None
 */
uint8_t getPWMFlag(void)
{
    return flag_pwm_update;
}

/**
 * @brief Sets the flag_pwm_update global variable
 *
 * @param value Value to place in the flag_pwm_update global variable
 *
 * @return None
 */
void setPWMFlag(uint8_t value)
{
    flag_pwm_update = value;
    return;
}

/**
 * @brief Gets the flag_one_second global variable
 *
 * @param None
 *
 * @return None
 */
uint8_t getOneSecFlag(void)
{
    return flag_one_second;
}

/**
 * @brief Sets the flag_one_second global variable
 *
 * @param value Value to place in the flag_one_second global variable
 *
 * @return None
 */
void setOneSecFlag(uint8_t value)
{
    flag_one_second = value;
    return;
}

/**
 * @brief Gets the tick_count global variable
 *
 * @param None
 *
 * @return None
 */
uint8_t getTickCount(void)
{
    return tick_count;
}

/**
 * @brief Increment the tick_count global variable by one
 *
 * @param None
 *
 * @return None
 */
void incrementTickCount(void)
{
    tick_count++;
    return;
}

/**
 * @brief Sets the tick_count global variable
 *
 * @param value Value to place in the tick_count global variable
 *
 * @return None
 */
void setTickCount(uint8_t value)
{
    tick_count = value;
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
