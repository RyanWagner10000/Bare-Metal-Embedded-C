/*
 * file: gpio.c
 * description: file that contains the helper functions for timers 2 & 5
 * author: Ryan Wagner
 * date: December 5, 2025
 * notes:
 */

#include "gpio.h"

uint32_t LED_BS13 = (1U << 13);
uint32_t LED_BR13 = (1U << 29);

/**
 * @brief Initiate the Green LED on pin on PD12
 *
 * @param None
 *
 * @return None
 */
void initGreenLED(void)
{
    // Enable clock access to GPIOD
    RCC->AHB1ENR |= GPIODEN;

    // Set PD12 mode to output
    GPIOD->MODER |= (1U << 24);
    GPIOD->MODER &= ~(1U << 25);

    return;
}

/**
 * @brief Initiate the Green LED on pin on PD13
 *
 * @param None
 *
 * @return None
 */
void initOrangeLED(void)
{
    // Enable clock access to GPIOD
    RCC->AHB1ENR |= GPIODEN;

    // Set PD13 mode to output
    GPIOD->MODER |= (1U << 26);
    GPIOD->MODER &= ~(1U << 27);

    return;
}

/**
 * @brief Initiate the Green LED on pin on PD14
 *
 * @param None
 *
 * @return None
 */
void initRedLED(void)
{
    // Enable clock access to GPIOD
    RCC->AHB1ENR |= GPIODEN;

    // Set PD14 mode to output
    GPIOD->MODER |= (1U << 28);
    GPIOD->MODER &= ~(1U << 29);

    return;
}

/**
 * @brief Initiate the Green LED on pin on PD15
 *
 * @param None
 *
 * @return None
 */
void initBlueLED(void)
{
    // Enable clock access to GPIOD
    RCC->AHB1ENR |= GPIODEN;

    // Set PD15 mode to output
    GPIOD->MODER |= (1U << 30);
    GPIOD->MODER &= ~(1U << 31);

    return;
}

/**
 * @brief Turn on LED
 *
 * @param led RED/BLUE/GREEN/ORANGE_LED variable
 *
 * @return None
 */
void onLED(uint32_t led)
{
    // Set pin High
    GPIOD->BSRR |= led;

    return;
}

/**
 * @brief Turn off LED
 *
 * @param led RED/BLUE/GREEN/ORANGE_LED variable
 *
 * @return None
 */
void offLED(uint32_t led)
{
    // Set pin High
    GPIOD->BSRR |= (led << 16);

    return;
}

/**
 * @brief Toggle LED to opposite state
 *
 * @param led RED/BLUE/GREEN/ORANGE_LED variable
 *
 * @return None
 */
void toggleLED(uint32_t led)
{
    // Set output of pin High/Low
    GPIOD->ODR ^= led;

    return;
}

/**
 * @brief Initialize the intigrated push-button on pin PA0
 *
 * @param None
 *
 * @return None
 */
void initPushButton(void)
{
    // Enable cloack access to GPIOA
    RCC->AHB1ENR |= GPIOAEN;

    // Set PA0 mode to input
    GPIOA->MODER &= ~(1U << 0);
    GPIOA->MODER &= ~(1U << 1);

    return;
}

/**
 * @brief Get the state of the push-button/if it's pressed or not
 *
 * @param None
 *
 * @return 1 if pressed, 0 otherwise
 */
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
