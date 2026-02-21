/*
 * file: spi3.c
 * description: file that contains the helper functions for SPI3 communciation protocol
 * author: Ryan Wagner
 * date: February 14, 2025
 * notes:
 */

#include "spi3.h"

/**
 * @brief Initalize SPI3 on PC10/11/12 with Chip Select on PC13 for IMU module
 *
 * @param None
 *
 * @return None
 *
 * @note Uses bus clock frequency / 16 for timing
 */
void initSPI3(void)
{
    // Enable clock to GPIOC
    RCC->AHB1ENR |= GPIOCEN;

    // Enable clock to SPI3
    RCC->APB1ENR |= SPI3_EN;

    // Set pin mode for PB10, PB11, PB12 to alternate function
    // Set pin mode for PB13 to output
    // Clear first
    GPIOC->MODER &= ~(3U << 20); // Clear 0b00
    GPIOC->MODER &= ~(3U << 22); // Clear 0b00
    GPIOC->MODER &= ~(3U << 24); // Clear 0b00
    GPIOC->MODER &= ~(3U << 26); // Clear 0b00
    // Then set
    GPIOC->MODER |= (2U << 20); // AF mode 0b10
    GPIOC->MODER |= (2U << 22); // AF mode 0b10
    GPIOC->MODER |= (2U << 24); // AF mode 0b10
    GPIOC->MODER |= (1U << 26); // Output mode 0b01

    // Set alternate function mode for PB10, PB11, PB12
    // Clear first
    GPIOC->AFRH &= ~(15U << 8);  // Clear 0b0000
    GPIOC->AFRH &= ~(15U << 12); // Clear 0b0000
    GPIOC->AFRH &= ~(15U << 16); // Clear 0b0000
    // Then set
    GPIOC->AFRH |= (5U << 8);  // AFRL10 -> 0b0101 = AF5
    GPIOC->AFRH |= (5U << 12); // AFRL11 -> 0b0101 = AF5
    GPIOC->AFRH |= (5U << 16); // AFRL12 -> 0b0101 = AF5

    // Initialize CS Pin to high
    GPIOC->ODR |= (1U << 13);

    // Clear config to init
    SPI3->CR1 = 0x0000;

    // Set clock to fPCLK/16 (BR = 011)
    SPI3->CR1 |= (3U << 3);  // Set BR = 011 = /16

    // Set CPHA and CPOL to 0 (Mode 0) to determine behavior
    SPI3->CR1 &= ~(3U << 0);

    // Set MSB first
    SPI3->CR1 &= ~(1U << 7);

    // Set mode to Master
    SPI3->CR1 |= (1U << 2);

    // Select software slave management SSM = 1 & SSI = 1
    SPI3->CR1 |= (1U << 9); // SSM
    SPI3->CR1 |= (1U << 8); // SSI

    // Turn on SPI3
    SPI3->CR1 |= (1U << 6);

    return;
}

/**
 * @brief Transmit messages on the SPI3 peripheral
 *
 * @param address Array of addresses to transmit
 * @param size Size of array messages to transmit
 *
 * @return None
 */
void transmitSPI3(uint8_t *address, uint32_t size)
{
    uint32_t i = 0;
    uint8_t temp;

    while (i < size)
    {
        // Wait until TXE is set
        while (!(SPI3->SR & (1U << 1)))
            ;

        // Write data to register
        SPI3->DR = address[i];
        i++;
    }

    // Wait until TXE is set
    while (!(SPI3->SR & (1U << 1)))
        ;

    // Wait for BUSY flag to reset
    while ((SPI3->SR & (1U << 7)))
        ;

    // Clear OVR flag
    temp = SPI3->DR;
    temp = SPI3->SR;
    temp++;

    return;
}

/**
 * @brief Receives messages on the SPI3 peripheral
 *
 * @param address Array of addresses to recieve
 * @param size Size of array messages to recieve
 *
 * @return None
 */
void receiveSPI3(uint8_t *address, uint32_t size)
{
    while (size)
    {
        // Wait until transmit buffer is empty
        while (!(SPI3->SR & (1U << 1)))
            ;

        // Send dummy data
        SPI3->DR = 0;

        // Wait for RXNE FLAG to be set
        while (!(SPI3->SR & (1U << 0)))
            ;

        // Read data from register
        *address++ = (SPI3->DR);
        size--;
    }
    return;
}

/**
 * @brief Enable the Chip Select pin on GPIO PC13 to begin talking to radio module
 *
 * @param None
 *
 * @return None
 */
void enableCS_SPI3(void)
{
    // Turn on SPI to device
    GPIOC->ODR &= ~SPI3_CS;
    // Small delay
    for (volatile uint8_t i; i < 10; i++)
        ;

    return;
}

/**
 * @brief Disable the Chip Select pin on GPIO PC13 to stop talking to radio module
 *
 * @param None
 *
 * @return None
 */
void disableCS_SPI3(void)
{
    // Small delay
    for (volatile uint8_t i; i < 10; i++)
        ;
    // Turn off SPI to device
    GPIOC->ODR |= SPI3_CS;
    // Small delay
    for (volatile uint8_t i; i < 10; i++)
        ;

    return;
}
