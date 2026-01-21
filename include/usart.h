/*
 * file: usart.h
 * description: header file for USART peripheral structure
 * author: Ryan Wagner
 * date: December 5, 2025
 * notes:
 */

#ifndef USART_H
#define USART_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"
#include "printing.h"

#define USART2_BASE 0x40004400
#define BAUDRATE 9600
#define USART2_CLOCK SYS_CLOCK

#define USART2 ((USART_TypeDef *)USART2_BASE)

typedef struct
{
    volatile uint32_t SR;   // offset: 0x00
    volatile uint32_t DR;   // offset: 0x04
    volatile uint32_t BRR;  // offset: 0x08
    volatile uint32_t CR1;  // offset: 0x0C
    volatile uint32_t CR2;  // offset: 0x10
    volatile uint32_t CR3;  // offset: 0x14
    volatile uint32_t GTPR; // offset: 0x18
} USART_TypeDef;

void initUSART2(void);
void usartWriteChar(uint8_t value);
void usartWriteNumber(int16_t ch);
void usartWriteString(const char *string);
uint32_t usart2Read(void);

#endif // USART_H
