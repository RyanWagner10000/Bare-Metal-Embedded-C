/*
 * file: usart.c
 * description: file that contains the helper functions for usart 2
 * author: Ryan Wagner
 * date: November 27, 2025
 * notes:
 */

#include "usart.h"

static void usartWrite(int ch)
{
    // Make sure transmit data register is empty
    while (!(USART2->SR & SR_TXE))
    {
    }

    // Write to transmit data register
    USART2->DR = (ch & 0xFF);
}

int __io_putchar(int ch)
{
    usartWrite(ch);
    return ch;
}

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate)
{
    return ((periph_clk + (baudrate / 2U)) / baudrate);
}

static void uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate)
{
    USART2->BRR = compute_uart_bd(periph_clk, baudrate);
}

void initUSART(void)
{
    // Enable clock to GPIOD
    RCC->AHB1ENR |= GPIODEN;

    // Set PD5 to Alternate Function mode 7
    GPIOD->MODER &=~ (1U << 10);
    GPIOD->MODER |= (1U << 11);

    // Set alternate fucntion type to AF7 (UART2_TX)
    GPIOD->AFRL &=~ (15U << 28);
    GPIOD->AFRL |= (7U << 28);

    // Enable clock to UART2
    RCC->APB1ENR |= USART2EN;

    // Configure Baudrate
    uart_set_baudrate(CLOCK_FREQ, BAUDRATE);

    // Configure transfer direction
    USART2->CR1 = CR1_TE;

    // Enable UART Module
    USART2->CR1 |= CR1_UE;
}