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
#include "usart.h"
#include "gpio.h"
#include "rcc.h"

//  Bit mask for GPIOD pin 13
#define PIN13 (1U << 13)
//  Alias for PIN13 representing LED pin
#define ORANGE_LED PIN13

//  Bit mask for GPIOA pin 0
#define PIN0 (1U << 0)
//  Alias for PIN0 representing Blue push button
#define PUSH_BUTTON PIN0

#endif // MAIN_H
