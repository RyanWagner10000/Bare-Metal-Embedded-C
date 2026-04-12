/*
 * file: spi1.h
 * description: header file for SPI1 peripheral structure
 * author: Ryan Wagner
 * date: December 14, 2025
 * notes:
 */

#ifndef SPI1_H
#define SPI1_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"

#define SPI1_BASE 0x40013000
#define SPI1_EN (1U << 12)
#define SPI1_CS (1U << 7)
#define TXE (1U << 1)
#define RXNE (1U << 0)
#define BUSY (1U << 7)

#define SPI1 ((SPI1_TypeDef *)(SPI1_BASE))

typedef struct
{
    volatile uint32_t CR1;     // offset: 0x00
    volatile uint32_t CR2;     // offset: 0x04
    volatile uint32_t SR;      // offset: 0x08
    volatile uint32_t DR;      // offset: 0x0C
    volatile uint32_t CRCPR;   // offset: 0x10
    volatile uint32_t RXCRCR;  // offset: 0x14
    volatile uint32_t TXCRCR;  // offset: 0x18
    volatile uint32_t I2SCFGR; // offset: 0x1C
    volatile uint32_t I2SPR;   // offset: 0x20
} SPI1_TypeDef;

void initSPI1(void);
void transferSPI1(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t length);
void enableCS_SPI1(void);
void disableCS_SPI1(void);

#endif // SPI1_H
