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
#include "gpio.h"
#include "rcc.h"
#include "timer2_5.h"
#include "timer8.h"
#include "usart.h"
#include "berryIMU.h"
#include "fpu.h"
#include "timer6.h"
#include "nrf24.h"

#define RX_P0_CHANNEL 0
#define FLASH_FAIL 100
#define FLASH_SUCCESS 250

#endif // MAIN_H
