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

static uint8_t MAX_BUFFER_SIZE = 8;

/**
 * @brief Internal function to write byte to specific register on the imu module
 *
 * @param imu_num IMU number (typically 1 or 2)
 * @param address Register address on imu module
 * @param value Value to set register on the module
 *
 * @return None
 */
static void writeRegisterSingle(uint8_t imu_num, uint8_t address, uint8_t value)
{
    uint8_t tx_buffer[2] = {address, value};
    uint8_t rx_buffer[2] = {0, 0};

    if (imu_num == IMU1)
    {
        // Set CS pin LOW
        enableCS_SPI1();

        // Send write operation
        transferSPI1(tx_buffer, rx_buffer, 2);

        // Set CE pin HIGH
        disableCS_SPI1();
    }
    else if (imu_num == IMU2)
    {
        // Set CS pin LOW
        enableCS_SPI2();

        // Send write operation
        transferSPI2(tx_buffer, rx_buffer, 2);

        // Set CE pin HIGH
        disableCS_SPI2();
    }

    return;
}

/**
 * @brief Internal function to write N-many bytes to specific register on the imu module
 *
 * @param imu_num IMU number (typically 1 or 2)
 * @param address Register address on imu module
 * @param value Value array to set register on the module
 * @param length Length of value array
 *
 * @return None
 */
// static void writeRegisterMulti(uint8_t imu_num, uint8_t address, uint8_t *value, uint8_t length)
// {
//     uint8_t tx_buffer[MAX_BUFFER_SIZE];
//     uint8_t rx_buffer[MAX_BUFFER_SIZE];

//     length = length <= MAX_BUFFER_SIZE ? length : MAX_BUFFER_SIZE;

//     // Copy data into Tx buffer
//     tx_buffer[0] = address;
//     for (uint8_t i = 1; i < length + 1; i++)
//     {
//         tx_buffer[i] = value[i - 1];
//     }

//     if (imu_num == IMU1)
//     {
//         // Set CS pin LOW
//         enableCS_SPI1();

//         // Send write operation
//         transferSPI1(tx_buffer, rx_buffer, length + 1);

//         // Set CE pin HIGH
//         disableCS_SPI1();
//     }
//     else if (imu_num == IMU2)
//     {
//         // Set CS pin LOW
//         enableCS_SPI2();

//         // Send write operation
//         transferSPI2(tx_buffer, rx_buffer, length + 1);

//         // Set CE pin HIGH
//         disableCS_SPI2();
//     }

//     return;
// }

/**
 * @brief Internal function to read byte of specific register on the imu module
 *
 * @param imu_num IMU number (typically 1 or 2)
 * @param address Register address on imu module
 *
 * @return None
 */
static uint8_t readRegisterSingle(uint8_t imu_num, uint8_t address)
{
    // Format message
    uint8_t tx_buffer[2] = {address | READ_OPERATION, 0x00};
    uint8_t rx_buffer[2] = {0, 0};

    if (imu_num == IMU1)
    {
        // Set CS pin LOW
        enableCS_SPI1();

        // Send read operation
        transferSPI1(tx_buffer, rx_buffer, 2);

        // Set CE pin HIGH
        disableCS_SPI1();
    }
    else if (imu_num == IMU2)
    {
        // Set CS pin LOW
        enableCS_SPI2();

        // Send read operation
        transferSPI2(tx_buffer, rx_buffer, 2);

        // Set CE pin HIGH
        disableCS_SPI2();
    }

    return rx_buffer[1];
}

/**
 * @brief Internal function to read N-many bytes from specific register on the imu module
 *
 * @param imu_num IMU number (typically 1 or 2)
 * @param address Register address on imu module
 * @param values Values array to get from registers on the module
 * @param length Length of value array
 *
 * @return None
 */
static void readRegisterMulti(uint8_t imu_num, uint8_t address, uint8_t *values, uint8_t length)
{
    uint8_t tx_buffer[MAX_BUFFER_SIZE];
    uint8_t rx_buffer[MAX_BUFFER_SIZE];

    length = length <= MAX_BUFFER_SIZE ? length : MAX_BUFFER_SIZE;

    // Copy data into Tx buffer
    tx_buffer[0] = address | READ_OPERATION;
    for (uint8_t i = 1; i < length + 1; i++)
    {
        tx_buffer[i] = 0x00;
    }

    if (imu_num == IMU1)
    {
        // Set CS pin LOW
        enableCS_SPI1();

        // Send write operation
        transferSPI1(tx_buffer, rx_buffer, length + 1);

        // Set CE pin HIGH
        disableCS_SPI1();
    }
    else if (imu_num == IMU2)
    {
        // Set CS pin LOW
        enableCS_SPI2();

        // Send write operation
        transferSPI2(tx_buffer, rx_buffer, length + 1);

        // Set CE pin HIGH
        disableCS_SPI2();
    }

    // Transfer data into values array
    for (int8_t i = 0; i < length; i++)
    {
        values[i] = rx_buffer[i + 1];
    }

    return;
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
    writeRegisterSingle(IMU1, CTRL1_XL, 0x9B);
    // writeRegisterSingle(IMU2, CTRL1_XL, 0x9B);

    // Accelerometer Low & high pass filter, ODR/9 = 0b11001000
    writeRegisterSingle(IMU1, CTRL8_XL, 0xC8);
    // writeRegisterSingle(IMU2, CTRL8_XL, 0xC8);

    // Accelerometer Block Data update, incrememnt during multi-byte read
    writeRegisterSingle(IMU1, CTRL3_C, 0x44);
    // writeRegisterSingle(IMU2, CTRL3_C, 0x44);

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
    writeRegisterSingle(IMU1, CTRL2_G, 0x94);
    // writeRegisterSingle(IMU2, CTRL2_G, 0x94);

    // Gyroscopes high pass filter BW: 925 Hz
    writeRegisterSingle(IMU1, CTRL7_G, 0x70);
    // writeRegisterSingle(IMU2, CTRL7_G, 0x70);

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
    writeRegisterSingle(IMU1, MASTER_CONFIG, 0x01);
    // writeRegisterSingle(IMU2, MASTER_CONFIG, 0x01);

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
    // Should be 0x6A
    return readRegisterSingle(imu_num, WHO_AM_I);
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
    uint8_t status = readRegisterSingle(imu_num, STATUS_REG);
    
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
    uint8_t status = readRegisterSingle(imu_num, STATUS_REG);

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

    initAccelerometer();

    initGyroscope();

    // Set the mode to Continuous
    // Set FIFO ODR to 1.66 kHz
    writeRegisterSingle(IMU1, FIFO_CTRL5, 0x44);
    // writeRegisterSingle(IMU2, FIFO_CTRL5, 0x44);

    // // Enable gyroscope digital LPF = 0b00000010
    writeRegisterSingle(IMU1, CTRL4_C, 0x02);
    // writeRegisterSingle(IMU2, CTRL4_C, 0x02);

    // // Enable circular burst-mode: Gyroscope + accelerometer + mag registers
    writeRegisterSingle(IMU1, CTRL5_C, 0xE0);
    // writeRegisterSingle(IMU2, CTRL5_C, 0xE0);

    // // Gyroscopes low pass filter BW: 925 Hz
    writeRegisterSingle(IMU1, CTRL6_C, 0x03);
    // writeRegisterSingle(IMU2, CTRL6_C, 0x03);

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
    uint8_t data_buffer[] = {0, 0, 0, 0, 0, 0};
    readRegisterMulti(imu_num, OUTX_L_XL, data_buffer, 6);

    // Combine high and low bytes to form data
    xyz[0] = ((data_buffer[1] << 8) | data_buffer[0]);
    xyz[1] = ((data_buffer[3] << 8) | data_buffer[2]);
    xyz[2] = ((data_buffer[5] << 8) | data_buffer[4]);

    return;
}

/**
 * @brief Get 3-axis gyroscope data
 *
 * @param imu_num IMU number (typically 1 or 2)
 * @param xyz Array of values to hold 16-bit X, Y, and Z values
 *
 * @return None
 */
void getGyroData(uint8_t imu_num, int16_t *xyz)
{
    uint8_t data_buffer[] = {0, 0, 0, 0, 0, 0};
    readRegisterMulti(imu_num, OUTX_L_G, data_buffer, 6);

    // Combine high and low bytes to form data
    xyz[0] = ((data_buffer[1] << 8) | data_buffer[0]);
    xyz[1] = ((data_buffer[3] << 8) | data_buffer[2]);
    xyz[2] = ((data_buffer[5] << 8) | data_buffer[4]);

    return;
}

/**
 * @brief Reads data from accel registers and prints x, y, z
 *
 * @param imu_num IMU number (typically 1 or 2)
 *
 * @return None
 */
void logRawAccelData(uint8_t imu_num)
{
    int16_t data_buffer[3] = {0, 0, 0};

    // Get data from sensor
    getAccelData(imu_num, data_buffer);

    char x_str[MAX_INT_STRING];
    char y_str[MAX_INT_STRING];
    char z_str[MAX_INT_STRING];

    intToStr((int32_t)data_buffer[0], x_str);
    intToStr((int32_t)data_buffer[1], y_str);
    intToStr((int32_t)data_buffer[2], z_str);

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

/**
 * @brief Reads data from gyro registers and prints x, y, z
 *
 * @param imu_num IMU number (typically 1 or 2)
 *
 * @return None
 */
void logRawGyroData(uint8_t imu_num)
{
    int16_t data_buffer[3] = {0, 0, 0};

    // Get data from sensor
    getGyroData(imu_num, data_buffer);

    char x_str[MAX_INT_STRING];
    char y_str[MAX_INT_STRING];
    char z_str[MAX_INT_STRING];

    intToStr((int32_t)data_buffer[0], x_str);
    intToStr((int32_t)data_buffer[1], y_str);
    intToStr((int32_t)data_buffer[2], z_str);

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

    return;
}
