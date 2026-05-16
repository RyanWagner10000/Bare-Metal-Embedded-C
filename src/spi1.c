/*
 * file: spi1.c
 * description: file that contains the helper functions for SPI1 communciation protocol
 * author: Ryan Wagner
 * date: December 14, 2025
 * notes:
 */

#include "spi1.h"

/**
 * @brief Initalize SPI1 on PB3/4/5 with Chip Select on PB7 for IMU module
 *
 * @param None
 *
 * @return None
 *
 * @note Uses bus clock frequency / 16 for timing
 */
void initSPI1(void)
{

    // Enable clock to GPIOB
    RCC->AHB1ENR |= GPIOBEN;

    // Enable clock to SPI1
    RCC->APB2ENR |= SPI1_EN;

    // Set pin mode for PB3, PB4, PB5 to alternate function
    // Set pin mode for PB7 to output
    // Clear first
    GPIOB->MODER &= ~(3U << 6);  // Clear 0b00
    GPIOB->MODER &= ~(3U << 8);  // Clear 0b00
    GPIOB->MODER &= ~(3U << 10); // Clear 0b00
    GPIOB->MODER &= ~(3U << 14); // Clear 0b00
    // Then set
    GPIOB->MODER |= (2U << 6);  // AF mode 0b10
    GPIOB->MODER |= (2U << 8);  // AF mode 0b10
    GPIOB->MODER |= (2U << 10); // AF mode 0b10
    GPIOB->MODER |= (1U << 14); // Output mode 0b01

    // Set alternate function mode for PB3, PB4, PB5
    // Clear first
    GPIOB->AFRL &= ~(15U << 12); // Clear 0b0000
    GPIOB->AFRL &= ~(15U << 16); // Clear 0b0000
    GPIOB->AFRL &= ~(15U << 20); // Clear 0b0000
    // Then set
    GPIOB->AFRL |= (5U << 12); // AFRL3 -> 0b0101 = AF5
    GPIOB->AFRL |= (5U << 16); // AFRL4 -> 0b0101 = AF5
    GPIOB->AFRL |= (5U << 20); // AFRL5 -> 0b0101 = AF5

    // Initialize CS Pin to high
    GPIOB->ODR |= SPI1_CS;

    // Set output type to push-pull
    GPIOB->OTYPER &= ~(1U << 3);
    GPIOB->OTYPER &= ~(1U << 4);
    GPIOB->OTYPER &= ~(1U << 5);

    // Set high speed for SPI pins
    GPIOB->OSPEEDR |= (3U << 6);
    GPIOB->OSPEEDR |= (3U << 8);
    GPIOB->OSPEEDR |= (3U << 10);

    // No pull-up / pull-down
    GPIOB->PUPDR &= ~(3U << 6);
    GPIOB->PUPDR &= ~(3U << 8);
    GPIOB->PUPDR &= ~(3U << 10);

    // Clear config to init
    SPI1->CR1 = 0x0000;

    // Set clock to fPCLK/16 (BR = 011)
    // SPI1->CR1 |= (3U << 3); // Set BR = 011 = /16

    // Set clock to fPCLK/4 (BR = 001)
    SPI1->CR1 |= (1U << 3); // Set BR = 001 = /4

    // Set CPHA and CPOL to 1 (Mode 3) to determine behavior
    // SPI1->CR1 |= (3U << 0);
    SPI1->CR1 &= ~(3U << 0);

    // Enable FULL duplex
    // SPI1->CR1 &= ~(1U << 10);

    // Set MSB first
    SPI1->CR1 &= ~(1U << 7);

    // Set mode to Master
    SPI1->CR1 |= (1U << 2);

    // Select software slave management SSM = 1 & SSI = 1
    SPI1->CR1 |= (1U << 9); // SSM
    SPI1->CR1 |= (1U << 8); // SSI

    // Enable Tx and Rx DMA
    SPI1->CR2 |= (3U << 0);

    // Enable ISR
    NVIC_EnableIRQ(SPI1_IRQn);

    // Turn on SPI1
    SPI1->CR1 |= (1U << 6);

    return;
}

/**
 * @brief Transfer and recieve 1+ uint8 values to device
 *
 * @param tx_buffer Array of 8-bit values to transmit
 * @param rx_buffer Array of 8-bit values received
 * @param length Length of tx and rx buffer arrays
 *
 * @return None
 */
void transferSPI1(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t length)
{
    uint8_t i = 0;
    while (i < length)
    {
        // Wait until TXE is set
        while (!(SPI1->SR & TXE))
            ;

        // Write data to register
        SPI1->DR = tx_buffer[i];

        // Wait for RXNE to be set
        while (!(SPI1->SR & RXNE))
            ;

        // Read data from register
        rx_buffer[i] = (SPI1->DR) & 0xFF;

        ++i;
    }

    // Wait for BUSY flag to reset
    while ((SPI1->SR & BUSY))
        ;

    // Drain the RX buffer of the junk byte clocked in during TX
    while (SPI1->SR & (1U << 0))
    {
        (void)SPI1->DR;
    }
    // Clear OVR flag
    (void)SPI1->SR; // Clear OVR

    return;
}

/**
 * @brief Enable the Chip Select pin on GPIO PB7 to begin talking to IMU module
 *
 * @param None
 *
 * @return None
 */
void enableCS_SPI1(void)
{
    // Turn on SPI to device
    GPIOB->ODR &= ~(SPI1_CS);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Disable the Chip Select pin on GPIO PB7 to stop talking to IMU module
 *
 * @param None
 *
 * @return None
 */
void disableCS_SPI1(void)
{

    // Turn off SPI to device
    GPIOB->ODR |= SPI1_CS;

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}
