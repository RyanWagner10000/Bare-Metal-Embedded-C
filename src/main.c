/*
 * file: main.c
 * description: fail that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#include "main.h"

uint32_t DELAY_COUNT = 1000000/1;

int main(void)
{
    // Enable clock access to GPIOD
    RCC->AHB1ENR |= GPIODEN;

    // Set bit 26 to 1 for GPIO_D Mode Register
    GPIOD->MODER |= (1U << 26);
    GPIOD->MODER &= ~(1U << 27);

    while (1)
    {
        // Alternate PD13 (Orange LED Pin) high/low
        GPIOD->ODR ^= LED_PIN;
        for (uint32_t volatile i = 0; i < DELAY_COUNT; i++)
        {
        }

    }
}
