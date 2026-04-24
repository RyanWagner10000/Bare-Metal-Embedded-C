/*
 * file: dma.c
 * description: setup for DMA functionalilty
 * author: Ryan Wagner
 * date: April 22, 2026
 * notes:
 */

#include "dma.h"

// IMU data structures
int16_t raw_accel_data[6] = {0, 0, 0, 0, 0, 0};
int16_t raw_gyro_data[6] = {0, 0, 0, 0, 0, 0};

void initDMA(void)
{
    // Allow clock access to DMA1 and DMA2 on bus AHB1
    RCC->AHB1ENR |= (1U << 21);
    RCC->AHB1ENR |= (1U << 22);

    // Disable DMA Stream
    DMA2->S0CR &= ~(1U << 0);

    // Wait till DMA is disabled
    while ((DMA2->S0CR & (1U << 0)))
        ;

    // Enable Circular Mode
    DMA2->S0CR |= (1U << 8);

    // Set Memory data size to byte
    DMA2->S0CR &= !(1U << 13);
    DMA2->S0CR &= !(1U << 14);

    // Set Peripheral data size to byte
    DMA2->S0CR &= !(1U << 11);
    DMA2->S0CR &= !(1U << 12);

    // Enable memory addr increment
    DMA2->S0CR |= (1U << 10);

    // Set Peripheral address
    DMA2->S0PAR = (uint32_t)(&(SPI1->DR));

    // Set memory address
    DMA2->S0M0AR = (uint32_t)(&raw_accel_data);

    // Set number of transfers
    DMA2->S0NDTR = (uint16_t)6;

    // Enable DMA Stream
    DMA2->S0CR |= (1U << 0);

    // Enable DMA on SPI1 bus
    SPI1->CR2 |= (1U << 0);

    return;
}