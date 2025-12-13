/*
 * file: main.c
 * description: file that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#include "main.h"

uint32_t DELAY_COUNT = 1000000 / 10;

void initTIM2(void)
{
    // Enable clock access to General Purpose Timer 2
    RCC->APB1ENR |= TIM2_EN;

    // Set Prescaler
    TIM2->PSC = TIM2_PSC - 1;
    // Set Auto-reload Register
    TIM2->ARR = TIM2_ARR - 1;
    // Set counter to 0
    TIM2->CNT = 0;

    // Enable TIM2 on Control Register AFTER setting everything up
    TIM2->CR1 = TIM2_CR1_EN;

    return;
}

void wait(uint32_t num_milliseconds)
{
    // Wait 10 milliseconds N times
    for (uint32_t i = 0; i < num_milliseconds; ++i)
    {
        while (!(TIM2->SR & TIM2_SR_UIF)){}
        TIM2->SR &= ~TIM2_SR_UIF;
    }

    return;
}

int main(void)
{
    initGreenLED();
    initOrangeLED();
    initRedLED();
    initBlueLED();
    
    initTIM2();

    initPushButton();

    initUSART2();

    uint32_t button_state = getButtonState();

    while (1)
    {
        button_state = getButtonState();
        
        if (button_state)
        {
            usart2Write('a');
            // usart2Write('\r');
            usart2Write('\n');

            toggleLED(GREEN_LED);
            toggleLED(ORANGE_LED);
            toggleLED(RED_LED);
            toggleLED(BLUE_LED);

            wait(50);
        }
    }
    
}
