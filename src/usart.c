/*
 * file: usart.c
 * description: file that contains the helper functions for the USART2 peripheral
 * author: Ryan Wagner
 * date: December 5, 2025
 * notes:
 */

#include "usart.h"

/**
 * @brief Initialize the USART2 peripheral for pins PD5/6
 *
 * @param None
 *
 * @return None
 */
void initUSART2(void)
{
    // Allow clock access to USART2 on bus APB1
    RCC->APB1ENR |= (1U << 17);
    // Allow clock access to GPIOD on bus AHB1
    RCC->AHB1ENR |= (1U << 3);

    // Set USART2 PD5 & PD6 Pin Mode
    GPIOD->MODER |= (1U << 11);
    GPIOD->MODER &= ~(1U << 10);
    GPIOD->MODER |= (1U << 13);
    GPIOD->MODER &= ~(1U << 12);

    // Set USART2 alternate function mode
    // AF7 for PD5
    GPIOD->AFRL &= ~(1U << 23);
    GPIOD->AFRL |= (1U << 22);
    GPIOD->AFRL |= (1U << 21);
    GPIOD->AFRL |= (1U << 20);
    // AF7 for PD6
    GPIOD->AFRL &= ~(1U << 27);
    GPIOD->AFRL |= (1U << 26);
    GPIOD->AFRL |= (1U << 25);
    GPIOD->AFRL |= (1U << 24);

    // Configure baud: 104.1875 From table 137 page 984
    USART2->BRR = 0x0683;

    // Enable Tx and Rx on CR1
    USART2->CR1 |= (1U << 3);
    USART2->CR1 |= (1U << 2);

    // Enable USART2
    USART2->CR1 |= (1U << 13);

    return;
}

/**
 * @brief Write a "string" to the USART2 peripheral
 *
 * @param string Character array to "print" to the terminal
 *
 * @return None
 */
void usartWriteString(const char *string)
{
    int32_t str_len = getStringLength(string);
    char char_i = 0;

    for (int32_t i = 0; i < str_len; i++)
    {
        // Make sure the transmit data register is NOT empty
        while (!(USART2->SR & (1U << 7)))
            ;

        // Get first byte of of input
        char_i = string[i];
        USART2->DR = (char_i & 0xFF);

        // Wait until done transmitting
        while (!(USART2->SR & (1U << 6)))
            ;
    }

    return;
}

/**
 * @brief Write a single character to the USART2 peripheral
 *
 * @param character ASCII style character to "print" to the terminal
 *
 * @return None
 */
void usartWriteChar(uint8_t character)
{
    // Make sure the transmit data register is NOT empty
    while (!(USART2->SR & (1U << 7)))
        ;

    // Get first byte of of input
    USART2->DR = (character & 0xFF);

    // Wait until done transmitting
    while (!(USART2->SR & (1U << 6)))
        ;

    return;
}

/**
 * @brief Write an integer to the USART2 peripheral
 *
 * @param value Signed 16-bit integer to "print" to the terminal
 *
 * @return None
 */
void usartWriteNumber(int16_t value)
{
    char buffer[12]; // Max 10 digits for uint32_t + null terminator + 1 extra
    int i = 0;

    // Handle zero case
    if (value == 0)
    {
        usartWriteChar('0');
        return;
    }

    // Convert number to string (reversed)
    while (value > 0)
    {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    // Print in correct order (reverse the buffer)
    while (i > 0)
    {
        usartWriteChar(buffer[--i]);
    }
}

/**
 * @brief Read a value off the USART2 Data Register when the Status Register says it's ready
 *
 * @param None
 *
 * @return An unsigned 32-bit integer
 */
uint32_t usart2Read(void)
{
    // Make sure the receive data register is NOT empty
    while (!(USART2->SR & (1U << 5)))
        ;

    uint32_t data = USART2->DR;

    return data;
}
