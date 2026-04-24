/*
 * file: dma.h
 * description: header file for DMA peripheral structure
 * author: Ryan Wagner
 * date: April 22, 2026
 * notes:
 */

#ifndef DMA_H
#define DMA_H

#include <stdint.h>
#include "rcc.h"
#include "spi1.h"
#include "spi3.h"

#define DMA1_BASE 0x40026000
#define DMA2_BASE 0x40026400

#define DMA1 ((DMA_TypeDef *)(DMA1_BASE))
#define DMA2 ((DMA_TypeDef *)(DMA2_BASE))

typedef struct
{
    volatile uint32_t LISR;   // offset: 0x00
    volatile uint32_t HISR;   // offset: 0x04
    volatile uint32_t LIFCR;  // offset: 0x08
    volatile uint32_t HIFCR;  // offset: 0x0C
    volatile uint32_t S0CR;   // offset: 0x10
    volatile uint32_t S0NDTR; // offset: 0x14
    volatile uint32_t S0PAR;  // offset: 0x18
    volatile uint32_t S0M0AR; // offset: 0x1C
    volatile uint32_t S0M1AR; // offset: 0x20
    volatile uint32_t S0FCR;  // offset: 0x24
    volatile uint32_t S1CR;   // offset: 0x28
    volatile uint32_t S1NDTR; // offset: 0x2C
    volatile uint32_t S1PAR;  // offset: 0x30
    volatile uint32_t S1M0AR; // offset: 0x34
    volatile uint32_t S1M1AR; // offset: 0x38
    volatile uint32_t S1FCR;  // offset: 0x3C
    volatile uint32_t S2CR;   // offset: 0x40
    volatile uint32_t S2NDTR; // offset: 0x44
    volatile uint32_t S2PAR;  // offset: 0x48
    volatile uint32_t S2M0AR; // offset: 0x4C
    volatile uint32_t S2M1AR; // offset: 0x50
    volatile uint32_t S2FCR;  // offset: 0x54
    volatile uint32_t S3CR;   // offset: 0x58
    volatile uint32_t S3NDTR; // offset: 0x5C
    volatile uint32_t S3PAR;  // offset: 0x60
    volatile uint32_t S3M0AR; // offset: 0x64
    volatile uint32_t S3M1AR; // offset: 0x68
    volatile uint32_t S3FCR;  // offset: 0x6C
    volatile uint32_t S4CR;   // offset: 0x70
    volatile uint32_t S4NDTR; // offset: 0x74
    volatile uint32_t S4PAR;  // offset: 0x78
    volatile uint32_t S4M0AR; // offset: 0x7C
    volatile uint32_t S4M1AR; // offset: 0x80
    volatile uint32_t S4FCR;  // offset: 0x84
    volatile uint32_t S5CR;   // offset: 0x88
    volatile uint32_t S5NDTR; // offset: 0x8C
    volatile uint32_t S5PAR;  // offset: 0x90
    volatile uint32_t S5M0AR; // offset: 0x94
    volatile uint32_t S5M1AR; // offset: 0x98
    volatile uint32_t S5FCR;  // offset: 0x9C
    volatile uint32_t S6CR;   // offset: 0xA0
    volatile uint32_t S6NDTR; // offset: 0xA4
    volatile uint32_t S6PAR;  // offset: 0xA8
    volatile uint32_t S6M0AR; // offset: 0xAC
    volatile uint32_t S6M1AR; // offset: 0xB0
    volatile uint32_t S6FCR;  // offset: 0xB4
    volatile uint32_t S7CR;   // offset: 0xB8
    volatile uint32_t S7NDTR; // offset: 0xBC
    volatile uint32_t S7PAR;  // offset: 0xC0
    volatile uint32_t S7M0AR; // offset: 0xC4
    volatile uint32_t S7M1AR; // offset: 0xC8
    volatile uint32_t S7FCR;  // offset: 0xCC
} DMA_TypeDef;

void initDMA(void);

#endif // DMA_H
