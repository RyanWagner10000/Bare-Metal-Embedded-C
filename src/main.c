/*
 * file: main.c
 * description: fail that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#include <stdint.h>
#include "main.h"

uint32_t DELAY_COUNT = 1000000/10;

/**
 * @brief Sets the bit N at address
 *
 * @param reg Register address to modify
 * @param bit Bit to set in register
 *
 * @return None
 */
static inline void setBit(volatile uint32_t *reg, uint32_t bit)
{
    *reg |= bit;
}

/**
 * @brief Clear the bit N at address
 *
 * @param reg Register address to modify
 * @param bit Bit to clear in register
 *
 * @return None
 */
static inline void clearBit(volatile uint32_t *reg, uint32_t bit)
{
    *reg &= ~bit;
}

int main(void)
{
    // Enable clock access to GPIOD
    //    setBit(&RCC_AHB1EN_R, GPIODEN);
    RCC_AHB1EN_R |= GPIODEN;

    // Set bit 26 to 1 for GPIO_D Mode Register
    //    setBit(&GPIOD_MODE_R, 26);
    GPIOD_MODE_R |= (1U << 26);
    //    clearBit(&GPIOD_MODE_R, 27);
    GPIOD_MODE_R &= ~(1U << 27);

    while (1)
    {
        // Set PD13 (Orange LED Pin) high
        //        setBit(&GPIOD_OD_R, LED_PIN);
        GPIOD_OD_R |= LED_PIN;
        for (uint32_t volatile i = 0; i < DELAY_COUNT; i++)
        {
        }

        //        clearBit(&GPIOD_OD_R, LED_PIN);
        GPIOD_OD_R &= ~LED_PIN;
        for (uint32_t volatile i = 0; i < DELAY_COUNT; i++)
        {
        }
    }
}
