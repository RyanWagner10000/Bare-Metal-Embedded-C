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
    GPIOB->ODR |= (1U << 12);

    // Clear config to init
    SPI2->CR1 = 0x0000;

    // Set clock to fPCLK/16 (BR = 011)
    SPI2->CR1 |= (3U << 3); // Set BR = 011 = /16

    // Set CPHA and CPOL to 0 (Mode 0) to determine behavior
    SPI2->CR1 &= ~(3U << 0);

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

/**
 * @brief Transmit messages on the SPI2 peripheral
 *
 * @param address Array of addresses to transmit
 * @param size Size of array messages to transmit
 *
 * @return None
 */
void transmitSPI2(uint8_t *address, uint32_t size)
{
    uint32_t i = 0;
    uint8_t temp;

    while (i < size)
    {
        // Wait until TXE is set
        while (!(SPI2->SR & (1U << 1)))
            ;

        // Write data to register
        SPI2->DR = address[i];
        i++;
    }

    // Wait until TXE is set
    while (!(SPI2->SR & (1U << 1)))
        ;

    // Wait for BUSY flag to reset
    while ((SPI2->SR & (1U << 7)))
        ;

    // Clear OVR flag
    temp = SPI2->DR;
    temp = SPI2->SR;
    temp++;

    return;
}

/**
 * @brief Receives messages on the SPI2 peripheral
 *
 * @param data Array for recieve data
 * @param size Size of array to recieve
 *
 * @return None
 */
void receiveSPI2(uint8_t *data, uint32_t size)
{
    while (size)
    {
        // Wait until transmit buffer is empty
        while (!(SPI2->SR & (1U << 1)))
            ;

        // Send dummy data
        SPI2->DR = 0;

        // Wait for RXNE FLAG to be set
        while (!(SPI2->SR & (1U << 0)))
            ;

        // Read data from register
        *data++ = (SPI2->DR);
        size--;
    }
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
    GPIOB->ODR &= ~SPI2_CS;
    // Small delay
    for (volatile uint8_t i; i < 10; i++)
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
    // Small delay
    for (volatile uint8_t i; i < 10; i++)
        ;
    // Turn off SPI to device
    GPIOB->ODR |= SPI2_CS;
    // Small delay
    for (volatile uint8_t i; i < 10; i++)
        ;

    return;
}
