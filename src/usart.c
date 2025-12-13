/*
 * file: usart.c
 * description: file that contains the helper functions for timers 2 & 5
 * author: Ryan Wagner
 * date: December 5, 2025
 * notes:
 */

#include "usart.h"

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
    // (104.1875)_10 = (1101000.0011)_2
    // (11010000011)_2 = (683)_16
    USART2->BRR = 0x0683;
    // USART2->BRR = (208 << 4)|5;

    // Enable Tx and Rx on CR1
    USART2->CR1 |= (1U << 3);
    USART2->CR1 |= (1U << 2);

    // Enable USART2
    USART2->CR1 |= (1U << 13);

    return;
}

void usart2Write(uint32_t ch)
{
    // Make sure the transmit data register is NOT empty
    while (!(USART2->SR & (1U << 7)))
        ;

    // Get first byte of of input
    USART2->DR = (ch & 0xFF);

    // Enable transmission
    // USART2->CR1 |= (1U << 3);

    // Wait until done transmitting
    while (!(USART2->SR & (1U << 6)))
        ;

    // Disable transmission
    // USART2->CR1 &= ~(1U << 3);

    return;
}

uint32_t usart2Read(void)
{
    // Make sure the receive data register is NOT empty
    while (!(USART2->SR & (1U << 5)))
        ;

    // Enable reception
    // USART2->CR1 |= (1U << 2);

    uint32_t data = USART2->DR;

    // Disable reception
    // USART2->CR1 &= ~(1U << 2);

    return data;
}
