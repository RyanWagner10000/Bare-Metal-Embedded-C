/*
 * file: ICM20948.c
 * description: file that contains the helper functions to read data and initialize the ICM 20948 module
 * author: Ryan Wagner
 * date: December 18, 2025
 * notes:
 */

#include "ICM20948.h"

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
 * @brief Returns the ID of the magnetometer
 *
 * @param None
 *
 * @return None
 */
uint8_t getMagnetometerID(uint8_t imu_num)
{
    // Read AK09916 WHO_AM_I (should be 0x09)
    // writeRegisterSingle(imu_num, REG_BANK_SEL, BANK_THREE);

    // writeRegisterSingle(imu_num, I2C_SLV0_ADDR, (0x80 | MAG_ADDRESS)); // Read from 0x0C

    // writeRegisterSingle(imu_num, I2C_SLV0_REG, 0x01); // WIA2 register

    // writeRegisterSingle(imu_num, I2C_SLV0_CTRL, 0x81); // Read 1 byte

    // writeRegisterSingle(imu_num, REG_BANK_SEL, BANK_ZERO);

    return readRegisterSingle(imu_num, EXT_SLV_SENS_DATA_00);
}

/**
 * @brief Returns the ID of the Accelerometer and Gyroscope
 *
 * @param None
 *
 * @return None
 */
uint8_t getAccelGyroID(uint8_t imu_num)
{
    // Make sure we're on bank 0 for this register
    writeRegisterSingle(imu_num, REG_BANK_SEL, BANK_ZERO);

    // Read ICM20948 WHO_AM_I (should be 0xEA)
    return readRegisterSingle(imu_num, WHO_AM_I);
}

/**
 * @brief Breakout function for just Accelerometer setup
 *
 * @param None
 *
 * @return None
 */
uint8_t initAccelerometer(uint8_t imu_num)
{
    uint8_t success = 1;
    uint8_t test = 0xFF;

    // uint8_t accel_smplrt_div_1 = 0x00;
    uint8_t accel_smplrt_div_2 = 0x00;
    uint8_t accel_config = 0x01;
    // uint8_t accel_config_2 = 0x00;

    // Change bank to 2
    writeRegisterSingle(imu_num, REG_BANK_SEL, BANK_TWO);

    // Enable LPF for accelerometer
    writeRegisterSingle(imu_num, ACCEL_CONFIG, accel_config);
    test = readRegisterSingle(imu_num, ACCEL_CONFIG);
    if (test != accel_config)
    {
        success = 0;
    }
    test = 0xFF;

    // // Set upper sample rate divider to 0
    // writeRegisterSingle(imu_num, ACCEL_SMPLRT_DIV_1, accel_smplrt_div_1);
    // test = readRegisterSingle(imu_num, ACCEL_SMPLRT_DIV_1);
    // if (test != accel_smplrt_div_1)
    // {
    //     success = 0;
    // }
    // test = 0xFF;

    // Set lower sample rate divider to 10 - 1
    writeRegisterSingle(imu_num, ACCEL_SMPLRT_DIV_2, accel_smplrt_div_2);
    test = readRegisterSingle(imu_num, ACCEL_SMPLRT_DIV_2);
    if (test != accel_smplrt_div_2)
    {
        success = 0;
    }
    test = 0xFF;

    // // Enable decimation of 32 samples
    // writeRegisterSingle(imu_num, ACCEL_CONFIG_2, accel_config_2);
    // test = readRegisterSingle(imu_num, ACCEL_CONFIG_2);
    // if (test != accel_config_2)
    // {
    //     success = 0;
    // }

    // Change bank to 0
    writeRegisterSingle(imu_num, REG_BANK_SEL, BANK_ZERO);

    return success;
}

/**
 * @brief Breakout function for just Gyroscope setup
 *
 * @param None
 *
 * @return None
 */
uint8_t initGyroscope(uint8_t imu_num)
{
    uint8_t success = 1;
    uint8_t test = 0xFF;

    uint8_t gyro_config_1 = 0x01;
    uint8_t gyro_smplrt_div = 0x00;
    // uint8_t gyro_config_2 = 0x02;

    // Change bank to 2
    writeRegisterSingle(imu_num, REG_BANK_SEL, BANK_TWO);

    // Enable digital low pass filter = b00100001
    writeRegisterSingle(imu_num, GYRO_CONFIG_1, gyro_config_1);
    test = readRegisterSingle(imu_num, GYRO_CONFIG_1);
    if (test != gyro_config_1)
    {
        success = 0;
    }
    test = 0xFF;

    // Configure gyro sample rate divider
    writeRegisterSingle(imu_num, GYRO_SMPLRT_DIV, gyro_smplrt_div);
    test = readRegisterSingle(imu_num, GYRO_SMPLRT_DIV);
    if (test != gyro_smplrt_div)
    {
        success = 0;
    }
    // test = 0xFF;

    // Enable 8x averaging for gyroscope
    // writeRegisterSingle(imu_num, GYRO_CONFIG_2, gyro_config_2);
    // test = readRegisterSingle(imu_num, GYRO_CONFIG_2);
    // if (test != gyro_config_2)
    // {
    //     success = 0;
    // }

    // Change bank to 0
    writeRegisterSingle(imu_num, REG_BANK_SEL, BANK_ZERO);

    return success;
}

/**
 * @brief Breakout function for just Magnetometer setup
 *
 * @param None
 *
 * @return None
 */
uint8_t initMagnetometer(void)
{
    // // Change bank to 0
    // writeIsm20948(REG_BANK_SEL, BANK_ZERO);

    // // Enable I2C master mode
    // writeIsm20948(USER_CTRL, 0x20);

    // // Change bank to 3
    // writeIsm20948(REG_BANK_SEL, BANK_THREE);

    // // Set timing I2C Master Clock Frequency to 345.60 w/ 46.67% duty
    // writeIsm20948(I2C_MST_CTRL, 0x17);

    // Enable delay for SLV0
    // writeIsm20948(I2C_MST_DELAY_CTRL, 0x00);

    // Reset the magnetometer on SLV4

    // // Magnetometer write address for reset
    // writeIsm20948(I2C_SLV4_ADDR, MAG_ADDRESS);

    // // Set the destination register
    // writeIsm20948(I2C_SLV4_REG, CNTL3);

    // // Enable reset with value 1
    // writeIsm20948(I2C_SLV4_DO, 0x01);

    // // Enable transaction
    // writeIsm20948(I2C_SLV4_CTRL, 0x80);

    // // Configure I2C to read status and mag data
    // writeIsm20948(I2C_SLV0_ADDR, (0x80 | MAG_ADDRESS));

    // // Set the destination register
    // writeIsm20948(I2C_SLV0_REG, MAG_DATA);

    // // Now set magnetometer configuration on SLV4, continuous mode on SLV0

    // // Magnetometer write address for continuous mode
    // writeIsm20948(I2C_SLV4_ADDR, MAG_ADDRESS);

    // // Set the destination register
    // writeIsm20948(I2C_SLV4_REG, CNTL2);

    // // Enable continuous mode with value b00001000
    // writeIsm20948(I2C_SLV4_DO, (1U << 2));

    // // Enable transaction
    // writeIsm20948(I2C_SLV4_CTRL, 0x80);

    // // Enable transaction for 8 bytes; and wait a little longer
    // writeIsm20948(I2C_SLV0_CTRL, 0x88);

    // // Read magnetometer data
    // for (int8_t i = 0; i < 8; i++)
    // {
    //     // data_buffer[i] = readRegister(EXT_SLV_SENS_DATA_00 + i);
    //     usartWriteNumber((uint8_t)i);
    // }
    // return;

    // // Change bank to 0
    // writeIsm20948(REG_BANK_SEL, BANK_ZERO);

    return 0;
}

/**
 * @brief Initialize/configure the IMU and all sensors
 *
 * @param None
 *
 * @return None
 */
uint8_t initIMU(uint8_t imu_num)
{
    // Alternate IMU1 and IMU2 commands to initialize everything
    uint8_t success = 1;
    uint8_t test = 0xFF;

    uint8_t pwr_mgmt_1 = 0x01;
    uint8_t pwr_mgmt_2 = 0x00;

    // Reset ICM 20948
    writeRegisterSingle(imu_num, PWR_MGMT_1, 0x80);

    // Set to auto sense best clock source
    writeRegisterSingle(imu_num, PWR_MGMT_1, pwr_mgmt_1);
    test = readRegisterSingle(imu_num, PWR_MGMT_1);
    if (test != pwr_mgmt_1)
    {
        success = 0;
    }
    test = 0xFF;

    // ########## Accelerometer ##########
    success = initAccelerometer(imu_num);

    // ########## Gyroscope ##########
    success = initGyroscope(imu_num);

    // ########## Magnetometer ##########
    // initMagnetometer(imu_num);

    // Enable accelerometer and gyroscope
    writeRegisterSingle(imu_num, PWR_MGMT_2, pwr_mgmt_2);
    test = readRegisterSingle(imu_num, PWR_MGMT_2);
    if (test != pwr_mgmt_2)
    {
        success = 0;
    }

    // Change bank to 0
    writeRegisterSingle(imu_num, REG_BANK_SEL, BANK_ZERO);

    // if (getMagnetometerID(imu_num) == (uint8_t)0x09)
    // {
    //     usartWriteString("Magnetometer discovered!\n");
    // }

    return success;
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
    readRegisterMulti(imu_num, ACCEL_XOUT_H, data_buffer, 6);

    // Combine high and low bytes to form data
    xyz[0] = ((data_buffer[0] << 8) | data_buffer[1]);
    xyz[1] = ((data_buffer[2] << 8) | data_buffer[3]);
    xyz[2] = ((data_buffer[4] << 8) | data_buffer[5]);

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
    readRegisterMulti(imu_num, GYRO_XOUT_H, data_buffer, 6);

    // Combine high and low bytes to form data
    xyz[0] = ((data_buffer[0] << 8) | data_buffer[1]);
    xyz[1] = ((data_buffer[2] << 8) | data_buffer[3]);
    xyz[2] = ((data_buffer[4] << 8) | data_buffer[5]);

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
    // uint8_t data_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    // // Read magnetometer data
    // for (int8_t i = 0; i < 8; i++)
    // {
    //     // data_buffer[i] = readRegister(EXT_SLV_SENS_DATA_00 + i);
    //     usartWriteNumber((uint8_t)i);
    // }
    // return;

    // if (!(data_buffer[0] & 0x01))
    // {
    //     // Check if data is ready
    //     return;
    // }
    // else if (data_buffer[0] & 0x02)
    // {
    //     // Check if overrun warning
    //     usartWriteString("Warning: Mag data overrun\n");
    // }

    // // Combine high and low bytes to form data
    // int16_t x = ((data_buffer[2] << 8) | data_buffer[1]);
    // int16_t y = ((data_buffer[4] << 8) | data_buffer[3]);
    // int16_t z = ((data_buffer[6] << 8) | data_buffer[5]);

    // if (data_buffer[7] & 0x08)
    // {
    //     // Check if magnetic overflow
    //     usartWriteString("Warning: Mag overflow\n");
    //     return;
    // }

    // char x_str[MAX_INT_STRING];
    // char y_str[MAX_INT_STRING];
    // char z_str[MAX_INT_STRING];

    // intToStr((int32_t)x, x_str);
    // intToStr((int32_t)y, y_str);
    // intToStr((int32_t)z, z_str);

    // char concat[MAX_STRING_CONCAT];

    // strConcat("Magn: x = ", x_str, concat);
    // strConcat(concat, " | ", concat);

    // strConcat(concat, "y = ", concat);
    // strConcat(concat, y_str, concat);
    // strConcat(concat, " | ", concat);

    // strConcat(concat, "z = ", concat);
    // strConcat(concat, z_str, concat);
    // strConcat(concat, "\n", concat);

    // usartWriteString(concat);
}

/**
 * @brief Prints a formatted "string" of raw accelerometer values
 *
 * @param None
 *
 * @return None
 */
void logRawAccelerometer(uint8_t imu_num)
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
}

/**
 * @brief Prints a formatted "string" of raw gyroscope values
 *
 * @param None
 *
 * @return None
 */
void logRawGyroscope(uint8_t imu_num)
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
}
