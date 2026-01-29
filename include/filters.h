/*
 * file: complimentary_filter.h
 * description: header file for a complimentary filer for the accel and gyro on the 9 DOF IMU
 * author: Ryan Wagner
 * date: January 20, 2026
 * notes:
 */

#ifndef COMP_FILTER_H
#define COMP_FILTER_H

#include <stdint.h>
#include "printing.h"
#include "ICM20948.h"
#include <math.h>
#include <errno.h>

#define roll = (0.0)
#define pitch = (0.0)
#define x_offset = (0.0)
#define y_offset = (0.0)
#define alpha = (0.98)
// #define dt = ()

#define MAX_BUFFER_SIZE (100)
#define ACCEL_BUFFER_SIZE (50)
#define GYRO_BUFFER_SIZE (50)

#define FLOAT_PRECISION (2)
#define ALPHA (0.9)

#define GYRO_SCALE (0.007629395)
#define ACCEL_SCALE (0.000061035)


typedef struct
{
    int32_t sum;
    float avg;
    uint32_t pos;
    uint32_t buffer[MAX_BUFFER_SIZE];
    uint32_t length;
} Moving_Avg_Typedef;

int errno; // A simple global variable to hold the error number

void init_averages(void);
void update_moving_avg(uint8_t address);
void logSensorData(uint8_t address);
void calculate_attitude(float loop_frequency);
void log_attitude(void);

#endif // COMP_FILTER_H
