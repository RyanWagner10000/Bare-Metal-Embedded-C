/*
 * file: gpio.h
 * description: header file for GPIO peripheral structure
 * author: Ryan Wagner
 * date: November 22, 2025
 * notes:
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef struct
{
    volatile uint32_t MODER;   // offset: 0x00
    volatile uint32_t OTYPER;  // offset: 0x04
    volatile uint32_t OSPEEDR; // offset: 0x08
    volatile uint32_t PUPDR;   // offset: 0x0C
    volatile uint32_t IDR;     // offset: 0x10
    volatile uint32_t ODR;     // offset: 0x14
    volatile uint32_t BSRR;    // offset: 0x18
    volatile uint32_t LCKR;    // offset: 0x1C
    volatile uint32_t AFRL;    // offset: 0x20
    volatile uint32_t AFRH;    // offset: 0x24
} GPIO_TypeDef;

#endif // GPIO_H
