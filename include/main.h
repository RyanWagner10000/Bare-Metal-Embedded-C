/*
 * file: main.h
 * description: header file for main
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "../include/gpio.h"
#include "../include/rcc.h"

#define RCC_BASE 0x40023800
#define GPIOA_BASE 0x40020000
#define GPIOD_BASE 0x40020C00

#define RCC ((RCC_TypeDef *)RCC_BASE)
#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOD ((GPIO_TypeDef *)GPIOD_BASE)

//  Bit mask for enabling GPIOD (bit 3)
#define GPIODEN (1U << 3)
//  Bit mask for GPIOD pin 13
#define PIN13 (1U << 13)
//  Alias for PIN13 representing LED pin
#define ORANGE_LED PIN13

//  Bit mask for enabling GPIOA (bit 0)
#define GPIOAEN (1U << 0)
//  Bit mask for GPIOA pin 0
#define PIN0 (1U << 0)
//  Alias for PIN0 representing Blue push button
#define PUSH_BUTTON PIN0

#endif // MAIN_H
