/*
 * file: cpi.c
 * description: file that contains the helper functions for SPI communciation protocol
 * author: Ryan Wagner
 * date: December 14, 2025
 * notes:
 */

#include "spi.h"

void initSPI(void)
{
    // Enable clock to GPIOB
    RCC->AHB1ENR |= GPIOBEN;

    // Enable clock to SPI1
    RCC->APB2ENR |= SPI1_EN;

    // Set pin mode for PB3, PB4, PB5 to alternate function
    // Set pin mode for PB7 to output
    // Clear first
    GPIOB->MODER &= ~(3U << 6);  // Clear 00
    GPIOB->MODER &= ~(3U << 8);  // Clear 00
    GPIOB->MODER &= ~(3U << 10); // Clear 00
    GPIOB->MODER &= ~(3U << 14); // Clear 00
    // Then set
    GPIOB->MODER |= (2U << 6);  // AF mode 10
    GPIOB->MODER |= (2U << 8);  // AF mode 10
    GPIOB->MODER |= (2U << 10); // AF mode 10
    GPIOB->MODER |= (1U << 14); // Output mode 01

    // Set alternate function mode for PB3, PB4, PB5
    // Clear first
    GPIOB->AFRL &= ~(15U << 12); // Clear 0000
    GPIOB->AFRL &= ~(15U << 16); // Clear 0000
    GPIOB->AFRL &= ~(15U << 20); // Clear 0000
    // Then set
    GPIOB->AFRL |= (5U << 12); // AFRL3 -> 0101 = AF5
    GPIOB->AFRL |= (5U << 16); // AFRL4 -> 0101 = AF5
    GPIOB->AFRL |= (5U << 20); // AFRL5 -> 0101 = AF5

    // Initialize CS Pin to high
    GPIOB->ODR |= (1U << 7);

    // Clear config to init
    SPI1->CR1 = 0x0000;

    // Set clock to fPCLK/16 (BR = 011)
    SPI1->CR1 &= ~(7U << 3);  // Clear BR[2:0]
    SPI1->CR1 |= (3U << 3);   // Set BR = 011 = /16

    // Set CPHA and CPOL to 0 (Mode 0) to determine behavior
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

    // Turn on SPI1
    SPI1->CR1 |= (1U << 6);

    return;
}

void transmitSPI(uint8_t *address, uint32_t size)
{
    uint32_t i = 0;
    uint8_t temp;

    while (i < size)
    {
        // Wait until TXE is set
        while (!(SPI1->SR & (1U << 1)))
            ;

        // Write data to register
        SPI1->DR = address[i];
        i++;
    }

    // Wait until TXE is set
    while (!(SPI1->SR & (1U << 1)))
        ;

    // Wait for BUSY flag to reset
    while ((SPI1->SR & (1U << 7)))
        ;

    // Clear OVR flag
    temp = SPI1->DR;
    temp = SPI1->SR;
    temp++;

    return;
}

void receiveSPI(uint8_t *address, uint32_t size)
{
    while (size)
    {
        // Wait until transmit buffer is empty
        while (!(SPI1->SR & (1U << 1)))
            ;
            
        // Send dummy data
        SPI1->DR = 0;

        // Wait for RXNE FLAG to be set
        while (!(SPI1->SR & (1U << 0)));

        // Read data from register
        *address++ = (SPI1->DR);
        size--;
    }
    return;
}

void enableCS(void)
{
    // Turn on SPI to device
    GPIOB->ODR &= ~(1U << 7);
    return;
}

void disableCS(void)
{
    // Turn off SPI to device
    GPIOB->ODR |= (1U << 7);
    return;
}