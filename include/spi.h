/*
 * file: spi.h
 * description: header file for SPI peripheral structure
 * author: Ryan Wagner
 * date: December 14, 2025
 * notes:
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"

#define SPI1_BASE 0x40013000
#define SPI1_EN (1U << 12)

#define SPI1 ((SPI_TypeDef *)(SPI1_BASE))

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
} SPI_TypeDef;

void initSPI(void);
void transmitSPI(uint8_t *data, uint32_t size);
void receiveSPI(uint8_t *data, uint32_t size);
void enableCS(void);
void disableCS(void);

#endif // SPI_H
