/*
 * file: spi2.h
 * description: header file for SPI2 peripheral structure
 * author: Ryan Wagner
 * date: February 14, 2025
 * notes:
 */

#ifndef SPI2_H
#define SPI2_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"

#define SPI2_BASE 0x40003800
#define SPI2_EN (1U << 14)
#define SPI2_CS (1U << 12)

#define SPI2 ((SPI2_TypeDef *)(SPI2_BASE))

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
} SPI2_TypeDef;

void initSPI2(void);
void transmitSPI2(uint8_t *address, uint32_t size);
void receiveSPI2(uint8_t *data, uint32_t size);
void enableCS_SPI2(void);
void disableCS_SPI2(void);

#endif // SPI2_H
