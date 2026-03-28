/*
 * file: berryIMU.c
 * description: file that contains the helper functions to read data and initialize the BerryIMU v3 module
 * author: Ryan Wagner
 * date: February 13, 2025
 * notes:
 *   Inspriation taken from BerryIMU Ozzmaker Github
 *   https://github.com/ozzmaker/BerryIMU/blob/master/gyro_accelerometer_tutorial01_angles/IMU.c
 */

#include "berryIMU.h"

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

    // address++;
    // data++;

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

/**
 * @brief Breakout function for just Accelerometer setup
 *
 * @param None
 *
 * @return None
 */
void initAccelerometer(void)
{
    // Accelerometer ODR 3.3 kHz = 0b10011011
    writeBerryIMU(IMU1, CTRL1_XL, 0x9B);
    // writeBerryIMU(IMU2, CTRL1_XL, 0x9B);

    // Accelerometer Low & high pass filter, ODR/9 = 0b11001000
    writeBerryIMU(IMU1, CTRL8_XL, 0xC8);
    // writeBerryIMU(IMU2, CTRL8_XL, 0xC8);

    // Accelerometer Block Data update, incrememnt during multi-byte read
    writeBerryIMU(IMU1, CTRL3_C, 0x44);
    // writeBerryIMU(IMU2, CTRL3_C, 0x44);

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
    // Gyro ODR 3.3 kHz and 500 dps = 0b10010100
    // writeBerryIMU(IMU1, CTRL2_G, 0x94);
    writeBerryIMU(IMU2, CTRL2_G, 0x94);

    // Gyroscopes high pass filter BW: 925 Hz
    // writeBerryIMU(IMU1, CTRL7_G, 0x70);
    writeBerryIMU(IMU2, CTRL7_G, 0x70);

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
    // Turn master I2C on
    // writeBerryIMU(IMU1, MASTER_CONFIG, 0x01);
    writeBerryIMU(IMU2, MASTER_CONFIG, 0x01);

    return;
}

/**
 * @brief Returns the ID of the Accelerometer and Gyroscope
 *
 * @param imu_num IMU number (typically 1 or 2)
 *
 * @return None
 */
uint8_t getWhoAmIxlgy(uint8_t imu_num)
{
    // Read IMU ID for LSM6DSL
    uint8_t id_num;
    readBerryIMU(imu_num, WHO_AM_I, &id_num, 1);

    // Should be 0x6A
    return id_num;
}

/**
 * @brief Returns the status of the gyroscope data being available
 *
 * @param imu_num IMU number (typically 1 or 2)
 *
 * @return None
 */
uint8_t statusGyro(uint8_t imu_num)
{
    uint8_t status = 0;
    uint8_t address = STATUS_REG;
    readBerryIMU(imu_num, address, &status, 1);

    // Return 1st bit value
    return (status & (1 << 1));
}

/**
 * @brief Returns the status of the accelerometer data being available
 *
 * @param imu_num IMU number (typically 1 or 2)
 *
 * @return None
 */
uint8_t statusAccel(uint8_t imu_num)
{
    uint8_t status = 0;
    uint8_t address = STATUS_REG;
    readBerryIMU(imu_num, address, &status, 1);

    // Return 0th bit value
    return (status & (1 << 0));
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
    // Alternate IMU1 and IMU2 commands to initialize everything

    // initAccelerometer();

    // initGyroscope();

    // Set the mode to Continuous
    // Set FIFO ODR to 1.66 kHz
    // writeBerryIMU(IMU1, FIFO_CTRL5, 0x44);
    // writeBerryIMU(IMU2, FIFO_CTRL5, 0x44);

    // // Enable gyroscope digital LPF = 0b00000010
    // writeBerryIMU(IMU1, CTRL4_C, 0x02);
    // writeBerryIMU(IMU2, CTRL4_C, 0x02);

    // // Enable circular burst-mode: Gyroscope + accelerometer + mag registers
    // writeBerryIMU(IMU1, CTRL5_C, 0xE0);
    // writeBerryIMU(IMU2, CTRL5_C, 0xE0);

    // // Gyroscopes low pass filter BW: 925 Hz
    // writeBerryIMU(IMU1, CTRL6_C, 0x03);
    // writeBerryIMU(IMU2, CTRL6_C, 0x03);

    return;
}

/**
 * @brief Get 3-axis accelerometer data
 *
 * @param imu_num IMU number (typically 1 or 2)
 * @param xyz Array of values to hold 16-bit X, Y, and Z values
 *
 * @return None
 */
void getAccelData(uint8_t imu_num, int16_t *xyz)
{
    uint8_t low_high_values[] = {0, 0, 0, 0, 0, 0};
    uint8_t address = OUTX_L_XL;
    readBerryIMU(imu_num, address, low_high_values, 6);

    // Combine high and low bytes to form data
    xyz[0] = ((low_high_values[1] << 8) | low_high_values[0]);
    xyz[1] = ((low_high_values[3] << 8) | low_high_values[2]);
    xyz[2] = ((low_high_values[5] << 8) | low_high_values[4]);

    return;
}

/**
 * @brief Reads X, Y, and Z low/high registers
 *
 * @param imu_num IMU number (typically 1 or 2)
 * @param data Array for returned X, Y, and Z data
 *
 * @return None
 */
void logRawAccelData(uint8_t imu_num, int16_t *data)
{
    // Address for minimum accelerometer address
    uint8_t address = OUTX_L_XL;

    uint8_t data_buffer[6] = {0, 0, 0, 0, 0, 0};

    // Get data from sensor
    if (imu_num == IMU1)
    {
        // Enable comms
        enableCS_SPI1();

        // Send request to get accelerometer data
        transmitSPI1(&address, 1);

        // Receive said data
        receiveSPI1(data_buffer, 6);

        // Disable comms
        disableCS_SPI1();
    }
    else if (imu_num == IMU2)
    {
        // Enable comms
        enableCS_SPI2();

        // Send request to get accelerometer data
        transmitSPI2(&address, 1);

        // Receive said data
        receiveSPI2(data_buffer, 6);

        // Disable comms
        disableCS_SPI2();
    }
    // Else maybe a warning?

    // Combine high and low bytes to form data
    data[0] = ((data_buffer[1] << 8) | data_buffer[0]);
    data[1] = ((data_buffer[3] << 8) | data_buffer[2]);
    data[2] = ((data_buffer[5] << 8) | data_buffer[4]);

    char x_str[MAX_INT_STRING];
    char y_str[MAX_INT_STRING];
    char z_str[MAX_INT_STRING];

    intToStr((int32_t)data[0], x_str);
    intToStr((int32_t)data[1], y_str);
    intToStr((int32_t)data[2], z_str);

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

    return;
}