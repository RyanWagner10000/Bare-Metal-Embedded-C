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
#include "gp_timer.h"
#include "usart.h"
#include "adc.h"
#include "spi.h"
#include "ICM20948.h"
#include "fpu.h"

#define MAX_BUFFER_SIZE (100)

typedef struct
{
    int32_t sum;
    float avg;
    uint32_t pos;
    uint32_t buffer[MAX_BUFFER_SIZE];
    uint32_t length;
} Moving_Avg_Typedef;

#endif // MAIN_H
