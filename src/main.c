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

void init_peripherals(void)
{
    enable_faults();
    enable_fpu();

    initGreenLED();
    initOrangeLED();
    initRedLED();
    initBlueLED();

    initTIM2();

    initPushButton();

    initUSART2();

    initSPI();
    initICM20948();

    return;
}

int main(void)
{
    init_peripherals();

    uint32_t button_state = getButtonState();

    while (1)
    {
        button_state = getButtonState();

        if (button_state)
        {

            // Read accelerometer data starting from data start
            readAccel(ACCEL_DATA, data_buffer);
            // readAccel(GYRO_DATA, data_buffer);

            // Combine high and low bytes to form data
            accel_x = (int16_t) ((data_buffer[1] << 8) | data_buffer[0]);
            accel_y = (int16_t) ((data_buffer[3] << 8) | data_buffer[2]);
            accel_z = (int16_t) ((data_buffer[5] << 8) | data_buffer[4]);

            char accel_x_str[MAX_FLOAT_STRING];
            char accel_y_str[MAX_FLOAT_STRING];
            char accel_z_str[MAX_FLOAT_STRING];
            int afterpoint = 2;
            float_to_string(accel_x, accel_x_str, afterpoint);
            float_to_string(accel_y, accel_y_str, afterpoint);
            float_to_string(accel_z, accel_z_str, afterpoint);

            char concat_x[MAX_STRING_CONCAT];
            str_concat("Accel x: ", accel_x_str, concat_x);
            str_concat(concat_x, "\n", concat_x);

            char concat_y[MAX_STRING_CONCAT];
            str_concat("Accel x: ", accel_x_str, concat_y);
            str_concat(concat_y, "\n", concat_y);

            char concat_z[MAX_STRING_CONCAT];
            str_concat("Accel x: ", accel_x_str, concat_z);
            str_concat(concat_z, "\n", concat_z);

            usartWriteString(concat_x);
            usartWriteString(concat_y);
            usartWriteString(concat_z);

            toggleLED(ORANGE_LED);

            wait(10);
        }
    }
}
