/*
 * file: ICM20948.c
 * description: file that contains the helper functions to read data and initialize the ICM 20948 module
 * author: Ryan Wagner
 * date: December 18, 2025
 * notes:
 */

#include "ICM20948.h"

/**
 * @brief Write 8-bit value to register on IMU module via SPI
 *
 * @param address Address of register on IMU module
 * @param value value to write to register
 *
 * @return None
 */
void writeIsm20948(uint8_t address, uint8_t value)
{
    uint8_t data[2];

    // Enable multi-byte, place address into buffer
    data[0] = address;

    // Place data into buffer
    data[1] = value;

    // Pull CS line low to enable slave device
    enableCS();

    // Transmit data and address
    transmitSPI(data, 2);

    // Pull CS line high to disable slave device
    disableCS();
}

/**
 * @brief Read 8-bit value from register on IMU module via SPI
 *
 * @param address Address of register on IMU module
 *
 * @return 8-bit unsigned value from IMU register
 */
uint8_t readRegister(uint8_t address)
{
    uint8_t data;

    address |= READ_OPERATION;

    enableCS();
    transmitSPI(&address, 1);
    receiveSPI(&data, 1);
    disableCS();

    return data;
}

/**
 * @brief Output contents of register to temrinal via USART2
 *
 * @note This is primarily used ofr debugging purposes, just to make sure you're seeing the raw values you'd expect
 *
 * @param address Address of register on IMU module
 *
 * @return None
 */
void printRegister(uint8_t address)
{
    uint8_t data = readRegister(address);

    char data_str[MAX_FLOAT_STRING];
    char addr_str[MAX_FLOAT_STRING];
    char concat[MAX_STRING_CONCAT];
    int_to_str((int32_t)data, data_str);
    int_to_str((int32_t)address, addr_str);
    str_concat("Register ", addr_str, concat);
    str_concat(concat, " = ", concat);
    str_concat(concat, data_str, concat);
    str_concat(concat, "\n", concat);
    usartWriteString(concat);

    return;
}

/**
 * @brief Helper function to get all lower and upper X, Y, and Z values of sensor
 *
 * @param address Starting address of sensor xyz values
 * @param data array of 8-bit integers to hold raw values
 *
 * @return None
 */
void getXYZ(uint8_t address, uint8_t *data)
{
    // Enable communication by pulling line low
    enableCS();

    // Set read operation
    address |= READ_OPERATION;

    // Send communication to recieve acceleromter data
    transmitSPI(&address, 1);

    // Receive acceleromter data
    receiveSPI(data, 6);

    // Disable communication by pulling line high
    disableCS();

    return;
}

/**
 * @brief Initialize/configure the IMU and all sensors
 *
 * @param None
 *
 * @return None
 */
void initICM20948(void)
{
    uint32_t delay_time = 10;

    // Initialize SPI1 interface
    initSPI();

    // Reset ICM 20948
    writeIsm20948(PWR_MGMT_1, 0x80);
    delay_ms(delay_time);

    // Set to auto sense best clock source
    writeIsm20948(PWR_MGMT_1, 0x01);
    delay_ms(delay_time);

    // Enable accelerometer and gyroscope
    writeIsm20948(PWR_MGMT_2, 0x00);
    delay_ms(delay_time);

    // Change bank to 2
    writeIsm20948(REG_BANK_SEL, 0x20);
    delay_ms(delay_time);

    // ########## Accelerometer ##########

    // Set upper sample rate divider to 0
    writeIsm20948(ACCEL_SMPLRT_DIV_1, 0x00);
    delay_ms(delay_time);

    // Set lower sample rate divider to 10 - 1
    writeIsm20948(ACCEL_SMPLRT_DIV_2, 0x09);
    delay_ms(delay_time);

    // Enable LPF for accelerometer
    writeIsm20948(ACCEL_CONFIG, 0x29);
    delay_ms(delay_time);

    // Enable decimation of 32 samples
    writeIsm20948(ACCEL_CONFIG2, 0x03);
    delay_ms(delay_time);

    // ########## Gyroscope ##########

    // Configure gyro sample rate divider to 5 - 1
    writeIsm20948(GYRO_SMPLRT_DIV, 0x04);
    delay_ms(delay_time);

    // Enable digital low pass filter
    writeIsm20948(GYRO_CONFIG1, 0x21);
    delay_ms(delay_time);

    // Enable 8x averaging for gyroscope
    writeIsm20948(GYRO_CONFIG2, 0x03);
    delay_ms(delay_time);

    // Change bank to 0
    writeIsm20948(REG_BANK_SEL, 0x00);
    delay_ms(delay_time);

    // whoami should be 0xEA (234 decimal)
    // uint8_t whoami = readRegister(0x00);

    return;
}