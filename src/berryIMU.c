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
uint8_t initAccelerometer(void)
{
    uint8_t success = 1;
    uint8_t test = 0;

    uint8_t ctrl1_xl = 0x9B;
    uint8_t ctrl8_xl = 0xC8;
    uint8_t ctrl3_c = 0x44;

    // Accelerometer ODR 3.3 kHz = 0b10011011
    writeRegisterSingle(IMU1, CTRL1_XL, ctrl1_xl);
    // writeRegisterSingle(IMU2, CTRL1_XL, 0x9B);
    test = readRegisterSingle(IMU1, CTRL1_XL);
    if (test != ctrl1_xl)
    {
        success = 0;
    }

    // Accelerometer Low & high pass filter, ODR/9 = 0b11001000
    writeRegisterSingle(IMU1, CTRL8_XL, ctrl8_xl);
    // writeRegisterSingle(IMU2, CTRL8_XL, 0xC8);
    test = readRegisterSingle(IMU1, CTRL8_XL);
    if (test != ctrl8_xl)
    {
        success = 0;
    }

    // Accelerometer Block Data update, incrememnt during multi-byte read
    writeRegisterSingle(IMU1, CTRL3_C, ctrl3_c);
    // writeRegisterSingle(IMU2, CTRL3_C, 0x44);
    test = readRegisterSingle(IMU1, CTRL3_C);
    if (test != ctrl3_c)
    {
        success = 0;
    }

    return success;
}

/**
 * @brief Breakout function for just Gyroscope setup
 *
 * @param None
 *
 * @return None
 */
uint8_t initGyroscope(void)
{
    uint8_t success = 1;
    uint8_t test = 0;

    uint8_t ctrl2_g = 0x94;
    uint8_t ctrl7_g = 0x70;

    // Gyro ODR 3.3 kHz and 500 dps = 0b10010100
    writeRegisterSingle(IMU1, CTRL2_G, ctrl2_g);
    // writeRegisterSingle(IMU2, CTRL2_G, 0x94);
    test = readRegisterSingle(IMU1, CTRL2_G);
    if (test != ctrl2_g)
    {
        success = 0;
    }
    

    // Gyroscopes high pass filter and 1.04Hz cutoff
    writeRegisterSingle(IMU1, CTRL7_G, ctrl7_g);
    // writeRegisterSingle(IMU2, CTRL7_G, 0x70);
    test = readRegisterSingle(IMU1, CTRL7_G);
    if (test != ctrl7_g)
    {
        success = 0;
    }

    return success;
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
    // Enable Embedded Functions
    writeRegisterSingle(IMU1, CTRL10_C, 0x04);

    // Turn master I2C on and Pull-up Aux On
    // writeRegisterSingle(IMU1, MASTER_CONFIG, 0x09);
    writeRegisterSingle(IMU1, MASTER_CONFIG, 0x01);
    // writeRegisterSingle(IMU1, MASTER_CONFIG, 0x05);

    // Enable Access to Bank A registers
    writeRegisterSingle(IMU1, FUNC_CFG_ACCESS, (1U << 7));

    // Set Mag ON with SLV0

    // Set SLV0 Address, bit 0 = 0 write operation
    uint8_t address = (MAG_ADDRESS << 1) | 0x00;
    writeRegisterSingle(IMU1, SLV0_ADD, address);

    // Set SLV0 Sub Address
    writeRegisterSingle(IMU1, SLV0_SUBADD, CTRL_REG3);

    // Set Data to be written: Set magnetometer to continuous mode
    writeRegisterSingle(IMU1, DATAWRITE_SRC_MODE_SUB_SLV0, 0x00);

    // Set SLV0 Config Address: write operation 1 time
    writeRegisterSingle(IMU1, SLAVE0_CONFIG, 0x01);

    usartWriteString("HERE\n");

    // Wait till operation is complete
    while (!(readRegisterSingle(IMU1, FUNC_SRC1) & 0x01))
        ;

    usartWriteString("AFTER\n");

    // Set SLV0 Sub Address
    writeRegisterSingle(IMU1, SLV0_SUBADD, CTRL_REG1);

    // Set Data to be written: High performance mode, fast ODR = 300Hz
    writeRegisterSingle(IMU1, DATAWRITE_SRC_MODE_SUB_SLV0, 0x42);

    // Set SLV0 Config Address: write operation 1 time
    writeRegisterSingle(IMU1, SLAVE0_CONFIG, 0x01);

    // Wait till operation is complete
    while (!(readRegisterSingle(IMU1, FUNC_SRC1) & 0x01))
        ;

    // Disable Access to Bank A registers
    writeRegisterSingle(IMU1, FUNC_CFG_ACCESS, 0x00);

    return;
}

/**
 * @brief Returns the ID of the Accelerometer and Gyroscope sensor
 *
 * @param imu_num IMU number (typically 1 or 2)
 *
 * @return Value at WHO_AM_I register
 */
uint8_t getWhoAmIxlgy(uint8_t imu_num)
{
    // Should be 0x6A
    return readRegisterSingle(imu_num, WHO_AM_I);
}

/**
 * @brief Returns the ID of the Magnetometer sensor
 *
 * @param imu_num IMU number (typically 1 or 2)
 *
 * @return Value at WHO_AM_I register
 */
uint8_t getWhoAmIMag(uint8_t imu_num)
{
    // Enable Access to Bank A registers
    writeRegisterSingle(imu_num, FUNC_CFG_ACCESS, (1U << 7));

    // Set SLV0 Address, bit 0 = 1 read operation
    uint8_t address = (MAG_ADDRESS << 1) | 0x01;
    writeRegisterSingle(imu_num, SLV0_ADD, address);

    // Set SLV0 Sub Address
    writeRegisterSingle(imu_num, SLV0_SUBADD, WHO_AM_I_MAG);

    // Set SLV0 Config Address: read operation 1 time
    writeRegisterSingle(imu_num, SLAVE0_CONFIG, 0x01);

    // Wait till operation is complete
    while (!(readRegisterSingle(imu_num, FUNC_SRC1) & 0x01))
        ;

    uint8_t name = readRegisterSingle(imu_num, SENSORHUB1_REG);

    // Disable Access to Bank A registers
    writeRegisterSingle(imu_num, FUNC_CFG_ACCESS, 0x00);

    // Should be 0x3D or 61
    return name;
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
uint8_t initBerryIMU(void)
{
    // Alternate IMU1 and IMU2 commands to initialize everything
    uint8_t success = 1;
    uint8_t test = 0;

    uint8_t fifo_ctrl5 = 0x44;
    uint8_t ctrl4_c = 0x02;
    uint8_t ctrl5_c = 0xE0;
    uint8_t ctrl6_c = 0x03;

    success = initAccelerometer();

    success = initGyroscope();

    // initMagnetometer();

    // Set the mode to Continuous
    // Set FIFO ODR to 1.66 kHz
    writeRegisterSingle(IMU1, FIFO_CTRL5, fifo_ctrl5);
    // writeRegisterSingle(IMU2, FIFO_CTRL5, 0x44);
    test = readRegisterSingle(IMU1, FIFO_CTRL5);
    if (test != fifo_ctrl5)
    {
        success = 0;
    }

    // // Enable gyroscope digital LPF = 0b00000010
    writeRegisterSingle(IMU1, CTRL4_C, ctrl4_c);
    // writeRegisterSingle(IMU2, CTRL4_C, 0x02);
    test = readRegisterSingle(IMU1, CTRL4_C);
    if (test != ctrl4_c)
    {
        success = 0;
    }

    // // Enable circular burst-mode: Gyroscope + accelerometer + mag registers
    writeRegisterSingle(IMU1, CTRL5_C, ctrl5_c);
    // writeRegisterSingle(IMU2, CTRL5_C, 0xE0);
    test = readRegisterSingle(IMU1, CTRL5_C);
    if (test != ctrl5_c)
    {
        success = 0;
    }

    // // Gyroscopes low pass filter BW: 925 Hz
    writeRegisterSingle(IMU1, CTRL6_C, ctrl6_c);
    // writeRegisterSingle(IMU2, CTRL6_C, 0x03);
    test = readRegisterSingle(IMU1, CTRL6_C);
    if (test != ctrl6_c)
    {
        success = 0;
    }

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
 * @brief Get 3-axis magnetometer data
 *
 * @param imu_num IMU number (typically 1 or 2)
 * @param xyz Array of values to hold 16-bit X, Y, and Z values
 *
 * @return None
 */
void getMagData(uint8_t imu_num, int16_t *xyz)
{
    uint8_t data_buffer[] = {0, 0, 0, 0, 0, 0};
    readRegisterMulti(imu_num, OUT_MAG_RAW_X_L, data_buffer, 6);

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

/**
 * @brief Reads data from magnetometer registers and prints x, y, z
 *
 * @param imu_num IMU number (typically 1 or 2)
 *
 * @return None
 */
void logRawMagData(uint8_t imu_num)
{
    int16_t data_buffer[3] = {0, 0, 0};

    // Get data from sensor
    getMagData(imu_num, data_buffer);

    char x_str[MAX_INT_STRING];
    char y_str[MAX_INT_STRING];
    char z_str[MAX_INT_STRING];

    intToStr((int32_t)data_buffer[0], x_str);
    intToStr((int32_t)data_buffer[1], y_str);
    intToStr((int32_t)data_buffer[2], z_str);

    char concat[MAX_STRING_CONCAT];

    strConcat("Mag: x = ", x_str, concat);
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
