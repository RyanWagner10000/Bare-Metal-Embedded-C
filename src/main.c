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

int16_t value_x, value_y, value_z;

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

void initPeripherals(void)
{
    enable_faults();
    enable_fpu();

    initGreenLED();
    initOrangeLED();
    initRedLED();
    initBlueLED();

    initTIM2();

    initPushButton();

    toggleLED(GREEN_LED);
    for (uint32_t i = 0; i < 1000000; i++);

    initUSART2();

    initSPI();
    uint8_t whoami = initICM20948();

    char temp[MAX_FLOAT_STRING];
    int_to_str((int32_t)whoami, temp);
    char concat[MAX_STRING_CONCAT];
    str_concat("Module name: ", temp, concat);
    str_concat(concat, "\n", concat);

    usartWriteString(concat);

    toggleLED(GREEN_LED);
    for (uint32_t i = 0; i < 1000000; i++);

    return;
}

int main(void)
{
    initPeripherals();

    uint32_t button_state = getButtonState();
    uint8_t run = 0;

    while (1)
    {
        button_state = getButtonState();
        if (button_state)
        {
            run ++;
            run %= 2;
            wait(100);
        }

        if (run)
        {
            // Read accelerometer data starting from data start
            readAccel(ACCEL_DATA, data_buffer);
            // readGyro(GYRO_DATA, data_buffer);

            // Combine high and low bytes to form data
            value_x = (int16_t)((data_buffer[0] << 8) | data_buffer[1]);
            value_y = (int16_t)((data_buffer[2] << 8) | data_buffer[3]);
            value_z = (int16_t)((data_buffer[4] << 8) | data_buffer[5]);

            char x_str[MAX_FLOAT_STRING];
            char y_str[MAX_FLOAT_STRING];
            char z_str[MAX_FLOAT_STRING];

            // int afterpoint = 1;
            int_to_str((int32_t)value_x, x_str);
            int_to_str((int32_t)value_y, y_str);
            int_to_str((int32_t)value_z, z_str);

            char concat[MAX_STRING_CONCAT];
            str_concat("Value x: ", x_str, concat);
            str_concat(concat, " | ", concat);
            
            str_concat(concat, "Value y: ", concat);
            str_concat(concat, y_str, concat);
            str_concat(concat, " | ", concat);

            str_concat(concat, "Value z: ", concat);
            str_concat(concat, z_str, concat);
            str_concat(concat, "\n", concat);

            usartWriteString(concat);

            toggleLED(ORANGE_LED);
        }
    }
}
