/*
 * file: berryIMU.c
 * description: file that contains the helper functions to read data and initialize the BerryIMU v3 module
 * author: Ryan Wagner
 * date: February 13, 2025
 * notes:
 */

#include "berryIMU.h"

/**
 * @brief Breakout function for just Accelerometer setup
 *
 * @param None
 *
 * @return None
 */
void initAccelerometer(void)
{
    return;
}

/**
 * @brief Breakout function for just Gyroscope setup
 *
 * @param None
 *
 * @return None
 */
void initGyroscope(void)
{
    return;
}

/**
 * @brief Breakout function for just Magnetometer setup
 *
 * @param None
 *
 * @return None
 */
void initMagnetometer(void)
{
    return;
}

/**
 * @brief Initialize/configure the IMU and all sensors
 *
 * @param None
 *
 * @return None
 */
void initBerryIMU(void)
{
    // Set the mode to Continuous

    return;
}

/**
 * @brief Write 8-bit value to register on IMU module via given IMU number
 *
 * @param imu_num IMU number (typically 1 or 2)
 * @param address Address of register on IMU module
 * @param data 8-bit data to write to register
 *
 * @return None
 */
void writeBerryIMU(uint8_t imu_num, uint8_t address, uint8_t data)
{
    // 16-bit data container
    uint8_t msg[2];

    // Place address into buffer
    msg[0] = address;

    // Place data into buffer
    msg[1] = data;

    if (imu_num == IMU1)
    {
        // Pull CS line low to enable slave device
        enableCS_SPI1();

        // Transmit data and address
        transmitSPI1(msg, 2);

        // Pull CS line high to disable slave device
        disableCS_SPI1();
    }
    else if (imu_num == IMU2)
    {
        // Pull CS line low to enable slave device
        enableCS_SPI2();

        // Transmit data and address
        transmitSPI2(msg, 2);

        // Pull CS line high to disable slave device
        disableCS_SPI2();
    }
    // Else, maybe error or something?

    return;
}

/**
 * @brief Reads 8-bit value from register on IMU module via given IMU number
 *
 * @param imu_num IMU number (typically 1 or 2)
 * @param address Address of register on IMU module
 * @param data Array for returned data
 * @param size Size of data array
 *
 * @return None
 */
void readBerryIMU(uint8_t imu_num, uint8_t address, uint8_t *data, uint32_t size)
{
    // temporary, just to compile
    imu_num += 1;
    address += 1;

    // Configure address to be a read operation
    address |= READ_OPERATION;

    if (imu_num == IMU1)
    {
        // Pull CS line low to enable slave device
        enableCS_SPI1();

        // Transmit data and address
        transmitSPI1(&address, 1);

        // Read data being transmitted from device
        receiveSPI1(data, size);

        // Pull CS line high to disable slave device
        disableCS_SPI1();
    }
    else if (imu_num == IMU2)
    {
        // Pull CS line low to enable slave device
        enableCS_SPI2();

        // Transmit data and address
        transmitSPI2(&address, 1);

        // Read data being transmitted from device
        receiveSPI2(data, size);

        // Pull CS line high to disable slave device
        disableCS_SPI2();
    }
    // Else, maybe error or something?
}