/*
 * file: spi3.h
 * description: header file for SPI3 peripheral structure
 * author: Ryan Wagner
 * date: February 14, 2025
 * notes:
 */

#ifndef SPI3_H
#define SPI3_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"

#define SPI3_BASE 0x40003C00
#define SPI3_EN (1U << 15)
#define SPI3_CE (1U << 3)
#define SPI3_CSN (1U << 4)
#define RXNE (1U << 0)
#define TXE (1U << 1)
#define BUSY (1U << 7)

#define SPI3 ((SPI3_TypeDef *)(SPI3_BASE))

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
} SPI3_TypeDef;

void initSPI3(void);
void transferSPI3(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t length);
void enableCSN_SPI3(void);
void disableCSN_SPI3(void);
void enableCE_SPI3(void);
void disableCE_SPI3(void);

#endif // SPI3_H
