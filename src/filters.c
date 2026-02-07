/*
 * file: complimentary filter.c
 * description: file that contains the functions for the complimentary filter of the gyro and accel
 * author: Ryan Wagner
 * date: January 20, 2026
 * notes:
 */

#include "filters.h"

// Accelerometer data
Moving_Avg_Typedef accel_avg_x;
Moving_Avg_Typedef accel_avg_y;
Moving_Avg_Typedef accel_avg_z;

// Gyroscope data
Moving_Avg_Typedef gyro_avg_x;
Moving_Avg_Typedef gyro_avg_y;
Moving_Avg_Typedef gyro_avg_z;

// Magnetometer data
Moving_Avg_Typedef mag_avg_x;
Moving_Avg_Typedef mag_avg_y;
Moving_Avg_Typedef mag_avg_z;

// Accelerometer offsets
volatile int16_t accel_offset_x = 0;
volatile int16_t accel_offset_y = 0;
volatile int16_t accel_offset_z = 0;

// Gyroscope offsets
volatile int16_t gyro_offset_x = 0;
volatile int16_t gyro_offset_y = 0;
volatile int16_t gyro_offset_z = 0;

// Magnetometer offsets
volatile int16_t mag_offset_x = 0;
volatile int16_t mag_offset_y = 0;
volatile int16_t mag_offset_z = 0;

// Input buffer and temp variables
uint8_t data_buffer[6] = {0, 0, 0, 0, 0, 0};
volatile int16_t x, y, z = 0;
volatile float roll_angle = 0;
volatile float pitch_angle = 0;

/**
 * @brief Function to return error number for math.h library
 *
 * @param None
 *
 * @return Error number
 */
int *__errno(void)
{
    return &errno;
}

/**
 * @brief Initialize a moving average struct
 *
 * @param avg Moving Average Struct pointer
 * @param size Size of the moving average array
 *
 * @return None
 */
void initSingleAverage(Moving_Avg_Typedef *avg, uint8_t size)
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

/**
 * @brief Initialize all moving averages for accelerometer, gyroscope, and magnetometer
 *
 * @param None
 *
 * @return None
 */
void initMovingAverages(void)
{
    initSingleAverage(&accel_avg_x, ACCEL_BUFFER_SIZE);
    initSingleAverage(&accel_avg_y, ACCEL_BUFFER_SIZE);
    initSingleAverage(&accel_avg_z, ACCEL_BUFFER_SIZE);

    initSingleAverage(&gyro_avg_x, GYRO_BUFFER_SIZE);
    initSingleAverage(&gyro_avg_y, GYRO_BUFFER_SIZE);
    initSingleAverage(&gyro_avg_z, GYRO_BUFFER_SIZE);

    initSingleAverage(&mag_avg_x, MAG_BUFFER_SIZE);
    initSingleAverage(&mag_avg_y, MAG_BUFFER_SIZE);
    initSingleAverage(&mag_avg_z, MAG_BUFFER_SIZE);

    return;
}

/**
 * @brief Update a Moving Average Struct with a value
 *
 * @param avg Moving Average Struct pointer
 * @param value Value to add to the moving average
 *
 * @return None
 */
void updateAvg(Moving_Avg_Typedef *avg, int32_t value)
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

/**
 * @brief Update all the moving averages given a register on the IMU
 *
 * @param address IMU register address for accelerometer or gyroscope
 *
 * @return None
 */
void updateMovingAvg(uint8_t address)
{
    // Check to see if address is valid
    if (address != ACCEL_DATA && address != GYRO_DATA && address != MAG_DATA)
    {
        return;
    }

    // Read accelerometer data starting from data start
    getXYZ(address, data_buffer);

    // Combine high and low bytes to form data
    x = (int16_t)((data_buffer[0] << 8) | data_buffer[1]);
    y = (int16_t)((data_buffer[2] << 8) | data_buffer[3]);
    z = (int16_t)((data_buffer[4] << 8) | data_buffer[5]);

    if (address == ACCEL_DATA)
    {
        updateAvg(&accel_avg_x, (int32_t)x);
        updateAvg(&accel_avg_y, (int32_t)y);
        updateAvg(&accel_avg_z, (int32_t)z);
    }
    else if (address == GYRO_DATA)
    {
        updateAvg(&gyro_avg_x, (int32_t)x);
        updateAvg(&gyro_avg_y, (int32_t)y);
        updateAvg(&gyro_avg_z, (int32_t)z);
    }
    else if (address == MAG_DATA)
    {
        updateAvg(&mag_avg_x, (int32_t)x);
        updateAvg(&mag_avg_y, (int32_t)y);
        updateAvg(&mag_avg_z, (int32_t)z);
    }

    return;
}

/**
 * @brief Prints a formatted "string" of accel/gyro data to the USART2 peripheral
 *
 * @param address IMU register address for accelerometer or gyroscope
 *
 * @return None
 */
void logAvgSensorData(uint8_t address)
{
    float avg_x = 0.0;
    float avg_y = 0.0;
    float avg_z = 0.0;
    char header[4] = "____";

    if (address == ACCEL_DATA)
    {
        avg_x = accel_avg_x.avg;
        avg_y = accel_avg_y.avg;
        avg_z = accel_avg_z.avg;
        header[0] = 'A';
        header[1] = 'c';
        header[2] = 'c';
        header[3] = 'l';
    }
    else if (address == GYRO_DATA)
    {
        avg_x = gyro_avg_x.avg;
        avg_y = gyro_avg_y.avg;
        avg_z = gyro_avg_z.avg;
        header[0] = 'G';
        header[1] = 'y';
        header[2] = 'r';
        header[3] = 'o';
    }
    else if (address == MAG_DATA)
    {
        avg_x = mag_avg_x.avg;
        avg_y = mag_avg_y.avg;
        avg_z = mag_avg_z.avg;
        header[0] = 'M';
        header[1] = 'a';
        header[2] = 'g';
        header[3] = 'n';
    }
    else
    {
        return;
    }

    char x_str[MAX_FLOAT_STRING];
    char y_str[MAX_FLOAT_STRING];
    char z_str[MAX_FLOAT_STRING];

    floatToStr(avg_x, x_str, FLOAT_PRECISION);
    floatToStr(avg_y, y_str, FLOAT_PRECISION);
    floatToStr(avg_z, z_str, FLOAT_PRECISION);

    char concat[MAX_STRING_CONCAT];
    strConcat(header, ": ", concat);

    strConcat(concat, "x = ", concat);
    strConcat(concat, x_str, concat);
    strConcat(concat, " | ", concat);

    strConcat(concat, "y = ", concat);
    strConcat(concat, y_str, concat);
    strConcat(concat, " | ", concat);

    strConcat(concat, "z = ", concat);
    strConcat(concat, z_str, concat);
    strConcat(concat, "\n", concat);

    usartWriteString(concat);
}

/**
 * @brief Complimentary filter calculator for acceleromter and gyroscope on IMU
 *
 * @param loop_frequency Float value of update frequency
 *
 * @return None
 */
void calculateAttitude(float loop_frequency)
{
    // Read gyro data
    getXYZ(GYRO_DATA, data_buffer);

    // Combine high and low bytes to form data & apply offsets
    x = (int16_t)((data_buffer[0] << 8) | data_buffer[1]);
    x -= gyro_offset_x;
    y = (int16_t)((data_buffer[2] << 8) | data_buffer[3]);
    y -= gyro_offset_y;

    // Scale to deg/sec (for +/- 250dps range)
    float gyro_x_dps = x * GYRO_SCALE;
    float gyro_y_dps = y * GYRO_SCALE;

    // Read accel data
    getXYZ(ACCEL_DATA, data_buffer);

    // Combine high and low bytes to form data
    x = (int16_t)((data_buffer[0] << 8) | data_buffer[1]);
    y = (int16_t)((data_buffer[2] << 8) | data_buffer[3]);
    z = (int16_t)((data_buffer[4] << 8) | data_buffer[5]);

    // Scale to g's (for ±2g range)
    float x_g = x * ACCEL_SCALE;
    float y_g = y * ACCEL_SCALE;
    float z_g = z * ACCEL_SCALE;

    // Gyro integration
    float gyro_roll_delta = gyro_x_dps * loop_frequency;
    float gyro_pitch_delta = gyro_y_dps * loop_frequency;

    float gyro_roll_angle = roll_angle + gyro_roll_delta;
    float gyro_pitch_angle = pitch_angle + gyro_pitch_delta;

    // Acceleromteter angle calculation
    float accel_roll_angle = atan2f(y_g, z_g) * 57.2958;
    float accel_pitch_angle = atan2f(-x_g, sqrtf((y_g * y_g) + (z_g * z_g))) * 57.2958;

    roll_angle = ALPHA * gyro_roll_angle + (1 - ALPHA) * accel_roll_angle;
    pitch_angle = ALPHA * gyro_pitch_angle + (1 - ALPHA) * accel_pitch_angle;

    return;
}

/**
 * @brief Prints a formatted "string" of pitch and roll values from compimentary filter
 *
 * @param None
 *
 * @return None
 */
void logAttitude(void)
{
    char roll_str[MAX_FLOAT_STRING];
    char pitch_str[MAX_FLOAT_STRING];

    floatToStr(roll_angle, roll_str, FLOAT_PRECISION);
    floatToStr(pitch_angle, pitch_str, FLOAT_PRECISION);

    char concat[MAX_STRING_CONCAT];

    strConcat("Attitude: ", "roll = ", concat);
    strConcat(concat, roll_str, concat);
    strConcat(concat, " | ", concat);

    strConcat(concat, "pitch = ", concat);
    strConcat(concat, pitch_str, concat);
    strConcat(concat, " | ", concat);

    strConcat(concat, "\n", concat);

    usartWriteString(concat);
}
