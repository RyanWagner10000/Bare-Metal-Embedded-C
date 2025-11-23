/*
 * file: main.c
 * description: fail that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#include "main.h"

uint32_t DELAY_COUNT = 1000000 / 10;

uint32_t LED_BS13 = (1U << 13);
uint32_t LED_BR13 = (1U << 29);

void initLED(void)
{
    // Enable clock access to GPIOD
    RCC->AHB1ENR |= GPIODEN;

    // Set PD13 mode to output
    GPIOD->MODER |= (1U << 26);
    GPIOD->MODER &= ~(1U << 27);

    return;
}

void onLED(void)
{
    // Set PD13 High
    GPIOD->BSRR |= LED_BS13;

    return;
}

void offLED(void)
{
    // Set PD13 High
    GPIOD->BSRR |= LED_BR13;

    return;
}

void initPushButton(void)
{
    // Enable cloack access to GPIOC
    RCC->AHB1ENR |= GPIOAEN;

    // Set PD13 mode to output
    GPIOA->MODER &= ~(1U << 0);
    GPIOA->MODER &= ~(1U << 1);

    return;
}

uint32_t getButtonState(void)
{
    // Button is active low

    // Check if button is pushed
    if (GPIOA->IDR & PUSH_BUTTON)
    {
        return 1;
    }

    return 0;
}

int main(void)
{
    initLED();
    initPushButton();

    uint32_t button_state = getButtonState();

    while (1)
    {
        button_state = getButtonState();
        
        if (button_state)
        {
            onLED();
        }
        else
        {
            offLED();
        }
        // for (uint32_t volatile i = 0; i < DELAY_COUNT; i++)
        // {
        // }
    }
}
