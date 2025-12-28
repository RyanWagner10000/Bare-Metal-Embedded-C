/*
 * file: main.c
 * description: file that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#include "main.h"

uint32_t DELAY_COUNT = 1000000 / 10;
uint8_t data_buffer[6];

int16_t accel_x, accel_y, accel_z;

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
        while (!(TIM2->SR & TIM2_SR_UIF))
        {
        }
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

    initSPI();
    initICM20948();

    uint32_t button_state = getButtonState();

    while (1)
    {
        button_state = getButtonState();

        if (button_state)
        {

            toggleLED(GREEN_LED);
            
            // Read accelerometer data starting from data start
            readAccel(ACCEL_DATA, data_buffer);
            // readAccel(GYRO_DATA, data_buffer);

            toggleLED(RED_LED);

            // Combine high and low bytes to form data
            accel_x = (int16_t) ((data_buffer[1] << 8) | data_buffer[0]);
            accel_y = (int16_t) ((data_buffer[3] << 8) | data_buffer[2]);
            accel_z = (int16_t) ((data_buffer[5] << 8) | data_buffer[4]);

            toggleLED(ORANGE_LED);

            usartWriteChar('[');
            usartWriteChar('x');
            usartWriteChar('=');
            usartWriteNumber(accel_x);
            usartWriteChar(' ');
            usartWriteChar('|');
            usartWriteChar(' ');

            usartWriteChar('y');
            usartWriteChar('=');
            usartWriteNumber(accel_y);
            usartWriteChar(' ');
            usartWriteChar('|');
            usartWriteChar(' ');
            
            usartWriteChar('z');
            usartWriteChar('=');
            usartWriteNumber(accel_z);
            usartWriteChar(']');
            usartWriteChar('\n');

            toggleLED(BLUE_LED);

            wait(5);
        }
    }
}
