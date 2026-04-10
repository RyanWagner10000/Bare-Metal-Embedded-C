/*
 * file: spi2.c
 * description: file that contains the helper functions for SPI2 communciation protocol
 * author: Ryan Wagner
 * date: February 14, 2025
 * notes:
 */

#include "spi2.h"

/**
 * @brief Initalize SPI2 on PB13/14/15 with Chip Select on PB12 for IMU module
 *
 * @param None
 *
 * @return None
 *
 * @note Uses bus clock frequency / 16 for timing
 */
void initSPI2(void)
{
    // Enable clock to GPIOB
    RCC->AHB1ENR |= GPIOBEN;

    // Enable clock to SPI2
    RCC->APB1ENR |= SPI2_EN;

    // Set pin mode for PB13, PB14, PB15 to alternate function
    // Set pin mode for PB12 to output
    // Clear first
    GPIOB->MODER &= ~(3U << 24); // Clear 0b00
    GPIOB->MODER &= ~(3U << 26); // Clear 0b00
    GPIOB->MODER &= ~(3U << 28); // Clear 0b00
    GPIOB->MODER &= ~(3U << 30); // Clear 0b00
    // Then set
    GPIOB->MODER |= (1U << 24); // Output mode 0b01
    GPIOB->MODER |= (2U << 26); // AF mode 0b10
    GPIOB->MODER |= (2U << 28); // AF mode 0b10
    GPIOB->MODER |= (2U << 30); // AF mode 0b10

    // Set alternate function mode for PB13, PB14, PB15
    // Clear first
    GPIOB->AFRH &= ~(15U << 20); // Clear 0b0000
    GPIOB->AFRH &= ~(15U << 24); // Clear 0b0000
    GPIOB->AFRH &= ~(15U << 28); // Clear 0b0000
    // Then set
    GPIOB->AFRH |= (5U << 20); // AFRL13 -> 0b0101 = AF5
    GPIOB->AFRH |= (5U << 24); // AFRL14 -> 0b0101 = AF5
    GPIOB->AFRH |= (5U << 28); // AFRL15 -> 0b0101 = AF5

    // Initialize CS Pin to high
    GPIOB->ODR |= SPI2_CS;

    // Clear config to init
    SPI2->CR1 = 0x0000;

    // Set clock to fPCLK/16 (BR = 011)
    SPI2->CR1 |= (3U << 3);

    // Set CPHA and CPOL to 0 (Mode 0) to determine behavior
    // SPI2->CR1 &= ~(3U << 0);
    SPI2->CR1 |= (3U << 0);

    // Enable FULL duplex
    // SPI2->CR1 &= ~(1U << 10);

    // Set MSB first
    SPI2->CR1 &= ~(1U << 7);

    // Set mode to Master
    SPI2->CR1 |= (1U << 2);

    // Select software slave management SSM = 1 & SSI = 1
    SPI2->CR1 |= (1U << 9); // SSM
    SPI2->CR1 |= (1U << 8); // SSI

    // Turn on SPI2
    SPI2->CR1 |= (1U << 6);

    return;
}

void transferSPI2(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t length)
{
    uint8_t i = 0;
    while (i < length)
    {
        // Wait until TXE is set
        while (!(SPI2->SR & TXE))
            ;
        
        // Write data to register
        SPI2->DR = tx_buffer[i];

        // Wait for RXNE to be set
        while (!(SPI2->SR & RXNE))
            ;
        
        // Read data from register
        rx_buffer[i] = (SPI2->DR) & 0xFF;

        ++i;
    }

    // Wait for BUSY flag to reset
    while ((SPI2->SR & BUSY))
        ;
    
    // Drain the RX buffer of the junk byte clocked in during TX
    while (SPI2->SR & (1U << 0))
    {
        (void)SPI2->DR;
    }
    // Clear OVR flag
    (void)SPI2->SR; // Clear OVR

    return;
}

/**
 * @brief Enable the Chip Select pin on GPIO PB12 to begin talking to IMU module
 *
 * @param None
 *
 * @return None
 */
void enableCS_SPI2(void)
{
    // Turn on SPI to device
    GPIOB->ODR &= ~(SPI2_CS);
    // Small delay
    for (volatile uint32_t i = 0; i < 10000; i++)
        ;

    return;
}

/**
 * @brief Disable the Chip Select pin on GPIO PB12 to stop talking to IMU module
 *
 * @param None
 *
 * @return None
 */
void disableCS_SPI2(void)
{
    // Turn off SPI to device
    GPIOB->ODR |= SPI2_CS;
    // Small delay
    for (volatile uint32_t i = 0; i < 10000; i++)
        ;

    return;
}
