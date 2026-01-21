/*
 * file: ICM20948.c
 * description: file that contains the helper functions to read data and initialize the ICM 20948 module
 * author: Ryan Wagner
 * date: December 18, 2025
 * notes:
 */

#include "ICM20948.h"

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

uint8_t initICM20948(void)
{
    // Initialize SPI1 interface
    initSPI();

    // Reset ICM 20948
    writeIsm20948(PWR_MGMT_1, 0x80);
    for (uint32_t i = 0; i < 100000; i++)
        ;
    
    // Set to auto sense best clock source
    writeIsm20948(PWR_MGMT_1, 0x01);
    for (uint32_t i = 0; i < 10000; i++)
        ;

    // Enable accelerometer and gyroscope
    writeIsm20948(PWR_MGMT_2, 0x00);
    for (uint32_t i = 0; i < 10000; i++)
        ;

    // Change bank to 2
    writeIsm20948(REG_BANK_SEL, 0x20);
    for (uint32_t i = 0; i < 10000; i++)
        ;

    // ########## Accelerometer ##########

    // Set upper sample rate divider to 0
    writeIsm20948(ACCEL_SMPLRT_DIV_1, 0x00);
    for (uint32_t i = 0; i < 10000; i++)
        ;

    // Set lower sample rate divider to 10 - 1
    writeIsm20948(ACCEL_SMPLRT_DIV_2, 0x09);
    for (uint32_t i = 0; i < 10000; i++)
        ;

    // Enable LPF for accelerometer
    writeIsm20948(ACCEL_CONFIG, 0x29);
    for (uint32_t i = 0; i < 10000; i++)
        ;

    // Enable decimation of 32 samples
    writeIsm20948(ACCEL_CONFIG2, 0x03);
    for (uint32_t i = 0; i < 10000; i++)
        ;

    // ########## Gyroscope ##########

    // Configure gyro sample rate divider to 5 - 1
    writeIsm20948(GYRO_SMPLRT_DIV, 0x04);
    for (uint32_t i = 0; i < 10000; i++)
        ;

    // Enable digital low pass filter
    writeIsm20948(GYRO_CONFIG1, 0x21);
    for (uint32_t i = 0; i < 10000; i++)
        ;

    // Enable 8x averaging for gyroscope
    writeIsm20948(GYRO_CONFIG2, 0x03);
    for (uint32_t i = 0; i < 10000; i++)
        ;

    // Change bank to 0
    writeIsm20948(REG_BANK_SEL, 0x00);
    for (uint32_t i = 0; i < 10000; i++)
        ;

    // whoami should be 0xEA (234 decimal)
    uint8_t whoami = readRegister(0x00);

    return whoami;
}