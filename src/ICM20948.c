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
    intToStr((int32_t)data, data_str);
    intToStr((int32_t)address, addr_str);
    strConcat("Register ", addr_str, concat);
    strConcat(concat, " = ", concat);
    strConcat(concat, data_str, concat);
    strConcat(concat, "\n", concat);
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
 * @brief Prints the ID of the magnetometer
 *
 * @param None
 *
 * @return None
 */
void logMagnetometerID(void)
{
    uint32_t delay_time = 10;

    // Read AK09916 WHO_AM_I (should be 0x09)
    writeIsm20948(REG_BANK_SEL, 0x30);
    delayMillisecond(delay_time);

    writeIsm20948(I2C_SLV0_ADDR, (0x80 | MAG_ADDRESS)); // Read from 0x0C
    delayMillisecond(delay_time);

    writeIsm20948(I2C_SLV0_REG, 0x01); // WIA2 register
    delayMillisecond(delay_time);

    writeIsm20948(I2C_SLV0_CTRL, 0x81); // Read 1 byte
    delayMillisecond(delay_time);

    writeIsm20948(REG_BANK_SEL, 0x00);
    delayMillisecond(delay_time);

    uint8_t mag_id = readRegister(EXT_SLV_SENS_DATA_00); // Should be 0x09

    char id_str[MAX_INT_STRING];

    intToStr((int32_t)mag_id, id_str);

    char concat[MAX_STRING_CONCAT];

    strConcat("Mag ID = ", id_str, concat);
    strConcat(concat, "\n", concat);

    usartWriteString(concat);
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
    delayMillisecond(delay_time);

    // Set to auto sense best clock source
    writeIsm20948(PWR_MGMT_1, 0x01);
    delayMillisecond(delay_time);

    // Enable accelerometer and gyroscope
    writeIsm20948(PWR_MGMT_2, 0x00);
    delayMillisecond(delay_time);

    // Change bank to 2
    writeIsm20948(REG_BANK_SEL, 0x20);
    delayMillisecond(delay_time);

    // ########## Accelerometer ##########

    // Set upper sample rate divider to 0
    writeIsm20948(ACCEL_SMPLRT_DIV_1, 0x00);
    delayMillisecond(delay_time);

    // Set lower sample rate divider to 10 - 1
    writeIsm20948(ACCEL_SMPLRT_DIV_2, 0x09);
    delayMillisecond(delay_time);

    // Enable LPF for accelerometer
    writeIsm20948(ACCEL_CONFIG, 0x29);
    delayMillisecond(delay_time);

    // Enable decimation of 32 samples
    writeIsm20948(ACCEL_CONFIG2, 0x03);
    delayMillisecond(delay_time);

    // ########## Gyroscope ##########

    // Configure gyro sample rate divider to 5 - 1
    writeIsm20948(GYRO_SMPLRT_DIV, 0x04);
    delayMillisecond(delay_time);

    // Enable digital low pass filter
    writeIsm20948(GYRO_CONFIG1, 0x21);
    delayMillisecond(delay_time);

    // Enable 8x averaging for gyroscope
    writeIsm20948(GYRO_CONFIG2, 0x03);
    delayMillisecond(delay_time);

    // Change bank to 0
    writeIsm20948(REG_BANK_SEL, 0x00);
    delayMillisecond(delay_time);

    // ########## Magnetometer ##########

    // Enable I2C master mode
    writeIsm20948(USER_CNTL, 0x30);
    // writeIsm20948(USER_CNTL, 0x20);
    delayMillisecond(delay_time);

    // Change bank to 3
    writeIsm20948(REG_BANK_SEL, 0x30);
    delayMillisecond(delay_time);

    // Set timing I2C Master Clock Frequency to 345.60 w/ 46.67% duty
    writeIsm20948(I2C_MST_CTRL, 0x07);
    delayMillisecond(delay_time);

    // Reset the magnetometer

    // Magnetometer write address for reset
    writeIsm20948(I2C_SLV0_ADDR, MAG_ADDRESS);
    delayMillisecond(delay_time);

    // Set the destination register
    writeIsm20948(I2C_SLV0_REG, CNTL3);
    delayMillisecond(delay_time);

    // Enable reset with value 1
    writeIsm20948(I2C_SLV0_DO, 0x01);
    delayMillisecond(delay_time);

    // Enable transaction for 1 byte by setting bits 7 and 1; and wait a little longer
    writeIsm20948(I2C_SLV0_CTRL, 0x81);
    delayMillisecond(delay_time * 10);

    // Now set magnetometer configuration

    // Magnetometer write address for continuous mode
    writeIsm20948(I2C_SLV0_ADDR, MAG_ADDRESS);
    delayMillisecond(delay_time);

    // Set the destination register
    writeIsm20948(I2C_SLV0_REG, CNTL2);
    delayMillisecond(delay_time);

    // Enable continuous mode with value b00001000
    writeIsm20948(I2C_SLV0_DO, 0x08);
    delayMillisecond(delay_time);

    // Enable transaction for 1 byte by setting bits 7 and 1; and wait a little longer
    writeIsm20948(I2C_SLV0_CTRL, 0x81);
    delayMillisecond(delay_time * 10);

    // Configure I2C to read status and mag data
    writeIsm20948(I2C_SLV0_ADDR, (0x80 | MAG_ADDRESS));
    delayMillisecond(delay_time);

    // Set the destination register
    writeIsm20948(I2C_SLV0_REG, MAG_DATA);
    delayMillisecond(delay_time);

    // Enable transaction for 6 bytes; and wait a little longer
    writeIsm20948(I2C_SLV0_CTRL, 0x86);
    delayMillisecond(delay_time * 10);

    // Change bank to 0
    writeIsm20948(REG_BANK_SEL, 0x00);
    delayMillisecond(delay_time);



    // Read AK09916 WHO_AM_I (should be 0x09)
    writeIsm20948(REG_BANK_SEL, 0x30);
    delayMillisecond(delay_time);

    writeIsm20948(I2C_SLV0_ADDR, (0x80 | MAG_ADDRESS)); // Read from 0x0C
    delayMillisecond(delay_time);

    writeIsm20948(I2C_SLV0_REG, 0x01); // WIA2 register
    delayMillisecond(delay_time);

    writeIsm20948(I2C_SLV0_CTRL, 0x81); // Read 1 byte
    delayMillisecond(delay_time);

    writeIsm20948(REG_BANK_SEL, 0x00);
    delayMillisecond(delay_time);
    
    logMagnetometerID();

    return;
}

/**
 * @brief Prints a formatted "string" of raw magnetometer values
 *
 * @param None
 *
 * @return None
 */
void logRawMagnetometer(void)
{
    uint8_t data_buffer[6] = {0, 0, 0, 0, 0, 0};

    // Read magnetometer data
    getXYZ(EXT_SLV_SENS_DATA_00, data_buffer);

    // Combine high and low bytes to form data
    int16_t x = ((data_buffer[0] << 8) | data_buffer[1]);
    int16_t y = ((data_buffer[2] << 8) | data_buffer[3]);
    int16_t z = ((data_buffer[4] << 8) | data_buffer[5]);

    // int16_t x = ((data_buffer[1] << 8) | data_buffer[0]);
    // int16_t y = ((data_buffer[3] << 8) | data_buffer[2]);
    // int16_t z = ((data_buffer[5] << 8) | data_buffer[4]);

    char x_str[MAX_INT_STRING];
    char y_str[MAX_INT_STRING];
    char z_str[MAX_INT_STRING];

    intToStr((int32_t)x, x_str);
    intToStr((int32_t)y, y_str);
    intToStr((int32_t)z, z_str);

    char concat[MAX_STRING_CONCAT];

    strConcat("Magn: x = ", x_str, concat);
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
 * @brief Prints a formatted "string" of raw accelerometer values
 *
 * @param None
 *
 * @return None
 */
void logRawAccelerometer(void)
{
    uint8_t data_buffer[6] = {0, 0, 0, 0, 0, 0};

    // Read accelerometer data
    getXYZ(ACCEL_DATA, data_buffer);

    // Combine high and low bytes to form data
    int16_t x = ((data_buffer[0] << 8) | data_buffer[1]);
    int16_t y = ((data_buffer[2] << 8) | data_buffer[3]);
    int16_t z = ((data_buffer[4] << 8) | data_buffer[5]);

    char x_str[MAX_INT_STRING];
    char y_str[MAX_INT_STRING];
    char z_str[MAX_INT_STRING];

    intToStr((int32_t)x, x_str);
    intToStr((int32_t)y, y_str);
    intToStr((int32_t)z, z_str);

    char concat[MAX_STRING_CONCAT];

    strConcat("Accl: x = ", x_str, concat);
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
 * @brief Prints a formatted "string" of raw gyroscope values
 *
 * @param None
 *
 * @return None
 */
void logRawGyroscope(void)
{
    uint8_t data_buffer[6] = {0, 0, 0, 0, 0, 0};

    // Read accelerometer data
    getXYZ(GYRO_DATA, data_buffer);

    // Combine high and low bytes to form data
    int16_t x = ((data_buffer[0] << 8) | data_buffer[1]);
    int16_t y = ((data_buffer[2] << 8) | data_buffer[3]);
    int16_t z = ((data_buffer[4] << 8) | data_buffer[5]);

    char x_str[MAX_INT_STRING];
    char y_str[MAX_INT_STRING];
    char z_str[MAX_INT_STRING];

    intToStr((int32_t)x, x_str);
    intToStr((int32_t)y, y_str);
    intToStr((int32_t)z, z_str);

    char concat[MAX_STRING_CONCAT];

    strConcat("Gyro: x = ", x_str, concat);
    strConcat(concat, " | ", concat);

    strConcat(concat, "y = ", concat);
    strConcat(concat, y_str, concat);
    strConcat(concat, " | ", concat);

    strConcat(concat, "z = ", concat);
    strConcat(concat, z_str, concat);
    strConcat(concat, "\n", concat);

    usartWriteString(concat);
}
