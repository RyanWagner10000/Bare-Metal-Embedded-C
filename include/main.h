/*
 * file: main.h
 * description: header file for main
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#ifndef MAIN_H
#define MAIN_H

//  1: Define base address for peripherals
#define PERIPH_BASE (0x40007800UL)
//  2: Offset for AHB1 peripheral bus
#define AHB1PERIPH_OFFSET (0x00018800UL)
//  3: Base address for AHB1 peripherals = 0x4002 0000
#define AHB1PERIPH_BASE (PERIPH_BASE + AHB1PERIPH_OFFSET)
//  4: Offset for GPIOD
#define GPIOD_OFFSET (0x00000C00UL)
//  5: Base address for GPIOD = 0x4002 0C00
#define GPIOD_BASE (AHB1PERIPH_BASE + GPIOD_OFFSET)
//  6: Offset for RCC
#define RCC_OFFSET (0x00003800UL)
//  7: Base address for RCC = 0x4002 3800
#define RCC_BASE (AHB1PERIPH_BASE + RCC_OFFSET)
//  8: Offset for AHB1EN register
#define AHB1EN_R_OFFSET (0x30)
//  9: Address of AHB1EN register = 0x4002 3830
#define RCC_AHB1EN_R (*(volatile unsigned int *)(RCC_BASE + AHB1EN_R_OFFSET))
//  10: Offset for mode register
#define MODE_R_OFFSET (0x00UL)
//  11: Address of GPIOD mode register = 0x4002 0C00
#define GPIOD_MODE_R (*(volatile unsigned int *)(GPIOD_BASE + MODE_R_OFFSET))
//  12: Offset for output data register
#define OD_R_OFFSET (0x14UL)
//  13: Address of GPIOD output data register = 0x4002 0C14
#define GPIOD_OD_R (*(volatile unsigned int *)(GPIOD_BASE + OD_R_OFFSET))
//  14: Bit mask for enabling GPIOD (bit 3)
#define GPIODEN (1U << 3)
//  15: Bit mask for GPIOD pin 13
#define PIN13 (1U << 13)
//  16: Alias for PIN5 representing LED pin
#define LED_PIN PIN13

#include <stdint.h>

static inline void setBit(volatile uint32_t *reg, uint32_t bit);
static inline void clearBit(volatile uint32_t *reg, uint32_t bit);

#endif // MAIN_H
