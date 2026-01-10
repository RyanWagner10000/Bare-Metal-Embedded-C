/*
 * file: ICM20948.c
 * description: file that contains the helper functions to read data and initialize the ICM 20948 module
 * author: Ryan Wagner
 * date: December 18, 2025
 * notes:
 */

#include <ICM20948.h>

void readAccel(uint8_t address, uint8_t *data)
{
    // Set read operation
    address |= READ_OPERATION;

    // Enable communication by pulling line low
    enableCS();

    // Send communication to recieve acceleromter data
    transmitSPI(&address, 1);

    // Receive acceleromter data
    receiveSPI(data, 6);

    // Disable communication by pulling line high
    disableCS();
    
    return;
}

void readGyro(uint8_t address, uint8_t *data)
{
    // Set read operation
    address |= READ_OPERATION;

    // Enable communication by pulling line low
    enableCS();

    // Send communication to recieve x-axis acceleromter data
    transmitSPI(&address, 1);

    // Receive low x-axis acceleromter data
    receiveSPI(data, 6);

    // Disable communication by pulling line high
    disableCS();
    
    return;
}

void write_ism20948(uint8_t address, uint8_t value)
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

void initICM20948(void)
{
    // Initialize SPI1 interface
    initSPI();

    // Set any options on the ICM 20948
    write_ism20948(PWR_MGMT_1, 0x06);

    // Enable accelerometer and gyroscope
    write_ism20948(PWR_MGMT_2, 0x00);

    // Change block to 2
    write_ism20948(REG_BANK_SEL, 0x20);

    // Enable LPF for accelerometer
    write_ism20948(ACCEL_CONFIG, 0x02);
    // Enable decimation of 32 samples
    write_ism20948(ACCEL_CONFIG2, 0x03);

    // Change block to 0
    write_ism20948(REG_BANK_SEL, 0x00);

    return;
}