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

    initUSART2();

    initSPI();

    initICM20948();

    return;
}

void update_avg(Moving_Avg_Typedef *avg, int32_t value)
{
    // Get previous index
    uint32_t previous_idx = (avg->pos - 1 + avg->length) % avg->length;

    // Subtract oldest value
    avg->sum -= avg->buffer[previous_idx];
    // Add new value
    avg->sum += value;

    // Update buffer that holds all values
    avg->buffer[avg->pos] = value;

    // Update current buffer index
    avg->pos++;
    avg->pos %= avg->length;

    avg->avg = avg->sum / (float)avg->length;

    return;
}

void init_moving_average(Moving_Avg_Typedef *avg, uint8_t size)
{
    // Set everything to 0 to it's filled and ready to use
    avg->length = (uint32_t)size;
    avg->avg = 0;
    avg->pos = 0;
    avg->sum = 0;

    for (uint8_t i = 0; i < size; i++)
    {
        avg->buffer[i] = 0;
    }

    return;
}

void logAccelData(Moving_Avg_Typedef *avg_x, Moving_Avg_Typedef *avg_y, Moving_Avg_Typedef *avg_z)
{
    char x_str[MAX_FLOAT_STRING];
    char y_str[MAX_FLOAT_STRING];
    char z_str[MAX_FLOAT_STRING];
    uint8_t accel_data_buffer[6] = {0, 0, 0, 0, 0, 0};
    int16_t accel_x, accel_y, accel_z;

    // Read accelerometer data starting from data start
    getXYZ(ACCEL_DATA, accel_data_buffer);

    // Combine high and low bytes to form data
    accel_x = (int16_t)((accel_data_buffer[0] << 8) | accel_data_buffer[1]);
    accel_y = (int16_t)((accel_data_buffer[2] << 8) | accel_data_buffer[3]);
    accel_z = (int16_t)((accel_data_buffer[4] << 8) | accel_data_buffer[5]);

    update_avg(avg_x, (int32_t)accel_x);
    update_avg(avg_y, (int32_t)accel_y);
    update_avg(avg_z, (int32_t)accel_z);

    float_to_str(avg_x->avg, x_str, 2);
    float_to_str(avg_y->avg, y_str, 2);
    float_to_str(avg_z->avg, z_str, 2);

    char concat[MAX_STRING_CONCAT];
    str_concat("A_x: ", x_str, concat);
    str_concat(concat, " | ", concat);

    str_concat(concat, "A_y: ", concat);
    str_concat(concat, y_str, concat);
    str_concat(concat, " | ", concat);

    str_concat(concat, "A_z: ", concat);
    str_concat(concat, z_str, concat);
    str_concat(concat, "\n", concat);

    usartWriteString(concat);
}

void logGyroData(Moving_Avg_Typedef *avg_x, Moving_Avg_Typedef *avg_y, Moving_Avg_Typedef *avg_z)
{
    char x_str[MAX_FLOAT_STRING];
    char y_str[MAX_FLOAT_STRING];
    char z_str[MAX_FLOAT_STRING];
    uint8_t gyro_data_buffer[6];
    int16_t gyro_x, gyro_y, gyro_z;

    // Read accelerometer data starting from data start
    getXYZ(GYRO_DATA, gyro_data_buffer);

    // Combine high and low bytes to form data
    gyro_x = (int16_t)((gyro_data_buffer[0] << 8) | gyro_data_buffer[1]);
    gyro_y = (int16_t)((gyro_data_buffer[2] << 8) | gyro_data_buffer[3]);
    gyro_z = (int16_t)((gyro_data_buffer[4] << 8) | gyro_data_buffer[5]);

    update_avg(avg_x, (int32_t)gyro_x);
    update_avg(avg_y, (int32_t)gyro_y);
    update_avg(avg_z, (int32_t)gyro_z);

    float_to_str(avg_x->avg, x_str, 2);
    float_to_str(avg_y->avg, y_str, 2);
    float_to_str(avg_z->avg, z_str, 2);

    char concat[MAX_STRING_CONCAT];
    str_concat("G_x: ", x_str, concat);
    str_concat(concat, " | ", concat);

    str_concat(concat, "G_y: ", concat);
    str_concat(concat, y_str, concat);
    str_concat(concat, " | ", concat);

    str_concat(concat, "G_z: ", concat);
    str_concat(concat, z_str, concat);
    str_concat(concat, "\n", concat);

    usartWriteString(concat);
}

int main(void)
{
    initPeripherals();

    toggleLED(BLUE_LED);

    uint32_t button_state = getButtonState();
    uint8_t run = 0;

    // Accelerometer data
    Moving_Avg_Typedef accel_avg_x;
    init_moving_average(&accel_avg_x, 50);
    Moving_Avg_Typedef accel_avg_y;
    init_moving_average(&accel_avg_y, 50);
    Moving_Avg_Typedef accel_avg_z;
    init_moving_average(&accel_avg_z, 50);

    // Gyroscope data
    Moving_Avg_Typedef gyro_avg_x;
    init_moving_average(&gyro_avg_x, 50);
    Moving_Avg_Typedef gyro_avg_y;
    init_moving_average(&gyro_avg_y, 50);
    Moving_Avg_Typedef gyro_avg_z;
    init_moving_average(&gyro_avg_z, 50);

    while (1)
    {
        button_state = getButtonState();
        if (button_state)
        {
            run++;
            run %= 2;
            offLED(ORANGE_LED);
            wait(100);
        }

        if (run)
        {
            // Read and print accelerometer data
            // logAccelData(&accel_avg_x, &accel_avg_y, &accel_avg_z);

            // Read and print gyroscope data
            logGyroData(&gyro_avg_x, &gyro_avg_y, &gyro_avg_z);

            toggleLED(ORANGE_LED);
        }
    }
}

// MAIN PROGRAM FLOW:
// ==================

// Initialize:
//     - Setup clocks (system, timers, peripherals)
//     - Initialize USART for debugging
//     - Initialize SPI
//     - Initialize PWM outputs for motors (TIM1/TIM2/etc)
//     - Initialize receiver input (PWM/PPM/SBUS on timers)
//     - Initialize ICM20948
//     - Calibrate gyroscope (drone must be stationary!)
//     - Calibrate ESCs if needed
//     - Wait for pilot "ready" signal (throttle low, stick movement, etc)

// Main Loop:
//     WHILE forever:
//         - Check if it's time to run control loop (based on timer/counter)
//         - IF control_loop_time_reached:
//             - Read IMU sensors
//             - Calculate attitude (sensor fusion)
//             - Read receiver inputs
//             - Run PID controllers
//             - Mix outputs to motors
//             - Send PWM signals to motors
//             - Update telemetry/logging
//         - ELSE:
//             - Handle low-priority tasks (LED blinking, status checks, etc)
