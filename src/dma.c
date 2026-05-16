/*
 * file: dma.c
 * description: setup for DMA functionalilty
 * author: Ryan Wagner
 * date: April 22, 2026
 * notes:
 */

#include "dma.h"

#define IMU_BURST_LEN 14 // 1 addr byte + 6 accel + 6 gyro + 1 dummy

// // TX buffer: read command reused every transfer
// static uint8_t IMU_READ_XL_GYRO[IMU_BURST_LEN] = {
//     ACCEL_XOUT_H | READ_OPERATION,      // first address byte with read bit set
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // accel X/Y/Z
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // gyro X/Y/Z
//     0x00                                // trailing dummy
// };

// TX buffer: read command reused every transfer
static uint8_t IMU_READ_XL_GYRO[IMU_BURST_LEN] = {
    REG_BANK_SEL,
    BANK_ZERO,
    ACCEL_XOUT_H | READ_OPERATION, // first address byte with read bit set
    ACCEL_XOUT_L | READ_OPERATION, // accel X/Y/Z
    ACCEL_YOUT_H | READ_OPERATION,
    ACCEL_YOUT_L | READ_OPERATION,
    ACCEL_ZOUT_H | READ_OPERATION,
    ACCEL_ZOUT_L | READ_OPERATION,
    GYRO_XOUT_H | READ_OPERATION, // gyro X/Y/Z
    GYRO_XOUT_L | READ_OPERATION,
    GYRO_YOUT_H | READ_OPERATION,
    GYRO_YOUT_L | READ_OPERATION,
    GYRO_ZOUT_H | READ_OPERATION,
    GYRO_ZOUT_L | READ_OPERATION,
};

// Double buffers for RX
static uint8_t imu_rx_buf_A[IMU_BURST_LEN];
static uint8_t imu_rx_buf_B[IMU_BURST_LEN];
static volatile uint8_t *dma_write_buf = imu_rx_buf_A; // DMA writes here
static volatile uint8_t *imu_read_buf = imu_rx_buf_B;  // control loop reads here

volatile int8_t spi1_imu_dma_ready = 0;
volatile int8_t spi2_imu_dma_ready = 0;

/**
 * @brief Initalize DMA and streams for SPI1 and SPI2
 *
 * @param None
 *
 * @return None
 */
void initDMA(void)
{
    // Allow clock access to DMA1 and DMA2 on bus AHB1
    RCC->AHB1ENR |= (1U << 21);
    // RCC->AHB1ENR |= (1U << 22);

    // NVIC for SPI1_RX
    // Enable ISER[0] bit 58, and set to priority 0 (highest)
    // NVIC_SetPriority(DMA2_Stream2_IRQn, 0);
    // NVIC_EnableIRQ(DMA2_Stream2_IRQn);

    // NVIC for SPI1_TX
    // Enable ISER[0] bit 59, and set to priority 1 (second-highest)
    // NVIC_SetPriority(DMA2_Stream3_IRQn, 1);
    // NVIC_EnableIRQ(DMA2_Stream3_IRQn);

    // NVIC for SPI2_RX
    // Enable ISER[0] bit 14, and set to priority 0 (highest)
    NVIC_SetPriority(DMA1_Stream3_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Stream3_IRQn);

    // NVIC for SPI2_TX
    // Enable ISER[0] bit 15, and set to priority 1 (second-highest)
    NVIC_SetPriority(DMA1_Stream4_IRQn, 1);
    NVIC_EnableIRQ(DMA1_Stream4_IRQn);

    return;
}

/**
 * @brief Initialize DMA settings for SPI1
 *
 * @param None
 *
 * @return None
 */
void initDMA_SPI1(void)
{
    // -- Rx --

    // Disable DMA Stream
    DMA2->S2CR &= ~(1U << 0);

    // Wait till DMA is disabled
    while ((DMA2->S2CR & (1U << 0)))
        ;

    // Disable Circular Mode
    DMA2->S2CR &= ~(1U << 8);

    // Set Memory data size to byte
    DMA2->S2CR &= ~(1U << 13);
    DMA2->S2CR &= ~(1U << 14);

    // Set Peripheral data size to byte
    DMA2->S2CR &= ~(1U << 11);
    DMA2->S2CR &= ~(1U << 12);

    // Enable memory addr increment
    DMA2->S2CR |= (1U << 10);

    // Clear interrupt flags for stream 2: CDMEIFx, CTEIFx, CTCIFx
    DMA2->LIFCR = (1U << 18) | (1U << 19) | (1U << 21);

    // Set Peripheral address
    DMA2->S2PAR = (uint32_t)(&(SPI1->DR));

    // Enable DMA2 Stream: Channel 3
    DMA2->S2CR |= (1U << 25);
    DMA2->S2CR |= (1U << 26);
    DMA2->S2CR &= ~(1U << 27);

    // Enable Transfer Complete Interrupt
    DMA2->S2CR |= (1U << 4);

    // Set priority level to very high
    DMA2->S2CR |= (3U << 16);

    // Enable ISR
    NVIC_EnableIRQ(DMA2_Stream2_IRQn);

    // Enable DMA Stream
    DMA2->S2CR |= (1U << 0);

    // -- Tx --

    // Disable DMA Stream
    DMA2->S3CR &= ~(1U << 0);

    // Wait till DMA is disabled
    while ((DMA2->S3CR & (1U << 0)))
        ;

    // Disable Circular Mode
    DMA2->S3CR &= ~(1U << 8);

    // Set Memory data size to byte
    DMA2->S3CR &= ~(1U << 13);
    DMA2->S3CR &= ~(1U << 14);

    // Set Peripheral data size to byte
    DMA2->S3CR &= ~(1U << 11);
    DMA2->S3CR &= ~(1U << 12);

    // Enable memory addr increment
    DMA2->S3CR |= (1U << 10);

    // Clear interrupt flags for stream 3: CDMEIFx, CTEIFx, CTCIFx
    DMA2->LIFCR = (1U << 24) | (1U << 25) | (1U << 27);

    // Set Peripheral address
    DMA2->S3PAR = (uint32_t)(&(SPI1->DR));

    // Enable DMA2 Stream: Channel 3
    DMA2->S3CR |= (1U << 25);
    DMA2->S3CR |= (1U << 26);
    DMA2->S3CR &= ~(1U << 27);

    // Enable Transfer Complete Interrupt
    DMA2->S3CR |= (1U << 4);

    // Set priority level to high
    DMA2->S3CR |= (2U << 16);

    // Enable ISR
    NVIC_EnableIRQ(DMA2_Stream3_IRQn);

    // Enable DMA Stream
    DMA2->S3CR |= (1U << 0);

    return;
}

/**
 * @brief Initialize DMA settings for SPI2
 *
 * @param None
 *
 * @return None
 */
void initDMA_SPI2(void)
{
    // -- Rx --

    // Disable DMA Stream
    DMA1->S3CR &= ~(1U << 0);

    // Wait till DMA is disabled
    while ((DMA1->S3CR & (1U << 0)))
        ;

    // Disable Circular Mode
    DMA1->S3CR &= ~(1U << 8);

    // Set Memory data size to byte
    DMA1->S3CR &= ~(1U << 13);
    DMA1->S3CR &= ~(1U << 14);

    // Set Peripheral data size to byte
    DMA1->S3CR &= ~(1U << 11);
    DMA1->S3CR &= ~(1U << 12);

    // Enable memory addr increment
    DMA1->S3CR |= (1U << 10);

    // Enable peripheral to memory direction
    DMA1->S4CR &= ~(1U << 7);
    DMA1->S4CR &= ~(1U << 6);

    // Clear interrupt flags for stream 3: CDMEIFx, CTEIFx, CTCIFx
    DMA1->LIFCR |= (0x3D << 22);

    // Set Peripheral address
    DMA1->S3PAR = (uint32_t)(&(SPI2->DR));

    // Enable DMA1 Stream 3: Channel 0
    DMA1->S3CR &= ~(1U << 25);
    DMA1->S3CR &= ~(1U << 26);
    DMA1->S3CR &= ~(1U << 27);

    // Enable Transfer Complete Interrupt
    DMA1->S3CR |= (1U << 4);

    // Set priority level to very high
    DMA1->S3CR |= (3U << 16);

    // Enable DMA Stream
    // DMA1->S3CR |= (1U << 0);

    // Enable ISR
    NVIC_EnableIRQ(DMA1_Stream3_IRQn);

    // -- Tx --

    // Disable DMA Stream
    DMA1->S4CR &= ~(1U << 0);

    // Wait till DMA is disabled
    while ((DMA1->S4CR & (1U << 0)))
        ;

    // Disable Circular Mode
    DMA1->S4CR &= ~(1U << 8);

    // Set Memory data size to byte
    DMA1->S4CR &= ~(1U << 13);
    DMA1->S4CR &= ~(1U << 14);

    // Set Peripheral data size to byte
    DMA1->S4CR &= ~(1U << 11);
    DMA1->S4CR &= ~(1U << 12);

    // Enable memory addr increment
    DMA1->S4CR |= (1U << 10);

    // Enable memory to peripheral direction
    DMA1->S4CR &= ~(1U << 7);
    DMA1->S4CR |= (1U << 6);

    // Clear interrupt flags for stream 4
    DMA1->HIFCR |= (0x3D << 0);

    // Set Peripheral address
    DMA1->S4PAR = (uint32_t)(&(SPI2->DR));

    // Enable DMA1 Stream 4: Channel 0
    DMA1->S4CR &= ~(1U << 25);
    DMA1->S4CR &= ~(1U << 26);
    DMA1->S4CR &= ~(1U << 27);

    // Enable Transfer Complete Interrupt
    // DMA1->S4CR |= (1U << 4);

    // Set priority level to high
    DMA1->S4CR |= (2U << 16);

    // Enable ISR
    NVIC_EnableIRQ(DMA1_Stream4_IRQn);

    // Enable DMA Stream
    // DMA1->S4CR |= (1U << 0);

    return;
}

/**
 * @brief Start the process of getting Accelerometer and Gyroscope data from IMU via DMA
 *
 * @param imu_num IMU number (typically 1 or 2)
 *
 * @return None
 */
void imuDMATransfer(uint8_t imu_num)
{
    if (imu_num == IMU1)
    {
        enableCS_SPI1();

        // -- Rx --
        // Disable DMA Stream
        DMA2->S2CR &= ~(1U << 0);

        // Wait till DMA is disabled
        while ((DMA2->S2CR & (1U << 0)))
            ;

        // Set memory address
        DMA2->S2M0AR = (uint32_t)dma_write_buf;

        // Set number of transfers
        DMA2->S2NDTR = (uint32_t)IMU_BURST_LEN;

        // Enable DMA Stream
        DMA2->S2CR |= (1U << 0);

        // -- Tx --
        // Disable DMA Stream
        DMA2->S3CR &= ~(1U << 0);

        // Wait till DMA is disabled
        while ((DMA2->S3CR & (1U << 0)))
            ;

        // Set memory address
        DMA2->S3M0AR = (uint32_t)IMU_READ_XL_GYRO;

        // Set number of transfers
        DMA2->S3NDTR = (uint32_t)IMU_BURST_LEN;

        // Clear ALL flags for 2 & 3
        DMA2->LIFCR |= (0xF7D << 16);

        // Enable DMA Stream
        DMA2->S3CR |= (1U << 0);

        // Enable Tx & Rx DMA on SPI bus
        SPI1->CR2 |= (3U << 0);
    }
    else if (imu_num == IMU2)
    {
        // -- Rx --
        // Disable DMA Stream
        DMA1->S3CR &= ~(1U << 0);

        // Wait till DMA is disabled
        while ((DMA1->S3CR & (1U << 0)))
            ;

        // Set memory address
        // DMA1->S3M0AR = (uint32_t)dma_write_buf;
        DMA1->S3M0AR = (uint32_t)(uint8_t *)dma_write_buf;

        // Set number of transfers
        DMA1->S3NDTR = (uint32_t)IMU_BURST_LEN;

        // Clear ALL flags for 3
        DMA1->LIFCR |= (0x3D << 22);

        // -- Tx --
        // Disable DMA Stream
        DMA1->S4CR &= ~(1U << 0);

        // Wait till DMA is disabled
        while ((DMA1->S4CR & (1U << 0)))
            ;

        // Set memory address
        DMA1->S4M0AR = (uint32_t)IMU_READ_XL_GYRO;

        // Set number of transfers
        DMA1->S4NDTR = (uint32_t)IMU_BURST_LEN;

        // Clear ALL flags for 4
        DMA1->HIFCR |= (0x3D << 0);

        // Enable Rx DMA Stream
        DMA1->S3CR |= (1U << 0);

        // Enable Tx DMA Stream
        DMA1->S4CR |= (1U << 0);

        // Enable Tx & Rx DMA on SPI bus
        SPI2->CR2 |= (3U << 0);

        enableCS_SPI2();
    }

    return;
}

/**
 * @brief Get if IMU data is ready
 *
 * @param imu_num IMU number (typically 1 or 2)
 *
 * @return None
 */
uint8_t getIMUDataReady(uint8_t imu_num)
{
    if (imu_num == IMU1)
    {
        return spi1_imu_dma_ready;
    }
    else if (imu_num == IMU2)
    {
        return spi2_imu_dma_ready;
    }
    return 0;
}

/**
 * @brief Get if IMU data is ready
 *
 * @param imu_num IMU number (typically 1 or 2)
 * @param value Value to set it to
 *
 * @return None
 */
void setIMUDataReady(uint8_t imu_num, uint8_t value)
{
    if (imu_num == IMU1)
    {
        spi1_imu_dma_ready = value;
    }
    else if (imu_num == IMU2)
    {
        spi2_imu_dma_ready = value;
    }
    return;
}

/**
 * @brief Swap received data into another buffer for main loop to use
 *
 * @param None
 *
 * @return None
 */
void swapBuffers(void)
{
    volatile uint8_t *temp = dma_write_buf;
    dma_write_buf = imu_read_buf;
    imu_read_buf = temp;

    return;
}

/**
 * @brief Copy and format raw data XL into X, Y, and Z components
 *
 * @param accel_data Pointer to array to fill
 *
 * @return None
 */
void getXLDataBuffer(int16_t *accel_data)
{
    accel_data[0] = (int16_t)(imu_read_buf[1] << 8) | (int16_t)(imu_read_buf[2]);
    accel_data[1] = (int16_t)(imu_read_buf[3] << 8) | (int16_t)(imu_read_buf[4]);
    accel_data[2] = (int16_t)(imu_read_buf[5] << 8) | (int16_t)(imu_read_buf[6]);
    return;
}

/**
 * @brief Copy and format raw data XL into X, Y, and Z components
 *
 * @param gyro_data Pointer to array to fill
 *
 * @return None
 */
void getGyroDataBuffer(int16_t *gyro_data)
{
    gyro_data[0] = (int16_t)(imu_read_buf[7] << 8) | imu_read_buf[8];
    gyro_data[1] = (int16_t)(imu_read_buf[9] << 8) | imu_read_buf[10];
    gyro_data[2] = (int16_t)(imu_read_buf[11] << 8) | imu_read_buf[12];
    return;
}
