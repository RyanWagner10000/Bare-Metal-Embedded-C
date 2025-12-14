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

#define SPI_BASE 0x40012000

#define SPI1 ((SPI_TypeDef *)(SPI_BASE))

typedef struct
{
    volatile uint32_t SR; // offset: 0x00
} SPI_TypeDef;

void initSPI(void);
void writeSPI(void);
uint32_t readSPI(void);

#endif // SPI_H
