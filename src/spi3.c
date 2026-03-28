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
    RCC->AHB1ENR |= GPIODEN;

    // Enable clock to SPI3
    RCC->APB1ENR |= SPI3_EN;

    // Set pin mode for PB10, PB11, PB12 to alternate function
    // Set pin mode for PD1 to output CE
    // Set pin mode for PD2 to output CSN
    // Clear first
    GPIOC->MODER &= ~(3U << 20); // Clear 0b00
    GPIOC->MODER &= ~(3U << 22); // Clear 0b00
    GPIOC->MODER &= ~(3U << 24); // Clear 0b00
    GPIOD->MODER &= ~(3U << 2);  // Clear 0b00
    GPIOD->MODER &= ~(3U << 4);  // Clear 0b00

    // Then set
    GPIOC->MODER |= (2U << 20); // AF mode 0b10
    GPIOC->MODER |= (2U << 22); // AF mode 0b10
    GPIOC->MODER |= (2U << 24); // AF mode 0b10
    GPIOD->MODER |= (1U << 2);  // Output mode 0b01
    GPIOD->MODER |= (1U << 4);  // Output mode 0b01

    // Set alternate function mode for PB10, PB11, PB12
    // Clear first
    GPIOC->AFRH &= ~(15U << 8);  // Clear 0b0000
    GPIOC->AFRH &= ~(15U << 12); // Clear 0b0000
    GPIOC->AFRH &= ~(15U << 16); // Clear 0b0000
    // Then set
    GPIOC->AFRH |= (6U << 8);  // AFRL10 -> 0b0101 = AF5
    GPIOC->AFRH |= (6U << 12); // AFRL11 -> 0b0101 = AF5
    GPIOC->AFRH |= (6U << 16); // AFRL12 -> 0b0101 = AF5

    // Initialize CS Pin to high
    GPIOD->ODR |= SPI3_CSN;
    GPIOD->ODR |= (SPI3_CE);

    // Set output type to push-pull
    GPIOC->OTYPER &= ~(1U << 10);
    GPIOC->OTYPER &= ~(1U << 11);
    GPIOC->OTYPER &= ~(1U << 12);

    // Set high speed for SPI pins
    GPIOC->OSPEEDR |= (3U << 10);
    GPIOC->OSPEEDR |= (3U << 12);
    GPIOC->OSPEEDR |= (3U << 14);

    // No pull-up / pull-down
    GPIOC->PUPDR &= ~(3U << 10);
    GPIOC->PUPDR &= ~(3U << 12);
    GPIOC->PUPDR &= ~(3U << 14);

    // Clear config to init
    SPI3->CR1 = 0x0000;

    // Set clock to fPCLK/16 (BR = 011)
    SPI3->CR1 |= (3U << 3); // Set BR = 011 = /16

    // Set CPHA and CPOL to 0 (Mode 0) to determine behavior
    // SPI3->CR1 |= (3U << 0);
    SPI3->CR1 &= ~(3U << 0);

    // Enable FULL duplex
    // SPI1->CR1 &= ~(1U << 10);

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

    while (i < size)
    {
        // Wait until TXE is set
        while (!(SPI3->SR & (1U << 1)))
            ;

        // Write data to register
        usartWriteString("Writing to DR: ");
        usartWriteNumber((uint32_t)address[i]);
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
    // Drain the RX buffer of the junk byte clocked in during TX
    while (SPI3->SR & (1U << 0))
    {
        (void)SPI3->DR;
    }
    (void)SPI3->SR; // Clear OVR

    return;
}

/**
 * @brief Receives messages on the SPI3 peripheral
 *
 * @param data Array for recieve data
 * @param size Size of array messages to recieve
 *
 * @return None
 */
void receiveSPI3(uint8_t *data, uint32_t size)
{
    while (size)
    {
        // Wait until transmit buffer is empty
        while (!(SPI3->SR & (1U << 1)))
            ;

        // Send dummy data
        SPI3->DR = 0xFF;

        // Wait for RXNE FLAG to be set
        while (!(SPI3->SR & (1U << 0)))
            ;

        // Read data from register
        *data++ = (uint8_t)(SPI3->DR);
        size--;
    }

    // Wait for BUSY flag to reset
    while ((SPI3->SR & (1U << 7)))
        ;

    return;
}

/**
 * @brief Enable the SPI Chip Select pin on GPIO PC13 to begin talking to radio module
 *
 * @param None
 *
 * @return None
 */
void enableCSN_SPI3(void)
{
    // Turn on SPI to device
    GPIOD->ODR &= ~(SPI3_CSN);

    // Small delay
    for (volatile uint8_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Disable the SPI Chip Select pin on GPIO PC13 to stop talking to radio module
 *
 * @param None
 *
 * @return None
 */
void disableCSN_SPI3(void)
{
    // Turn off SPI to device
    GPIOD->ODR |= SPI3_CSN;

    // Small delay
    for (volatile uint8_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Enable the Chip Enable pin on GPIO PD1 to enable Tx/Rx Mode
 *
 * @param None
 *
 * @return None
 */
void enableCE_SPI3(void)
{
    // Turn on SPI to device
    GPIOD->ODR |= SPI3_CE;

    // Small delay
    for (volatile uint8_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Disable the Chip Enable pin on GPIO PD1 to disable Tx/Rx Mode
 *
 * @param None
 *
 * @return None
 */
void disableCE_SPI3(void)
{
    // Turn off SPI to device
    GPIOD->ODR &= ~(SPI3_CE);

    // Small delay
    for (volatile uint8_t i = 0; i < 10; i++)
        ;

    return;
}