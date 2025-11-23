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
#define GPIOD_BASE 0x40020C00

#define RCC ((RCC_TypeDef *)RCC_BASE)
#define GPIOD ((GPIO_TypeDef *)GPIOD_BASE)

//  14: Bit mask for enabling GPIOD (bit 3)
#define GPIODEN (1U << 3)
//  15: Bit mask for GPIOD pin 13
#define PIN13 (1U << 13)
//  16: Alias for PIN5 representing LED pin
#define LED_PIN PIN13

#endif // MAIN_H
