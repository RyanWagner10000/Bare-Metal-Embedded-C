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
// #include "berryIMU.h"
#include "ICM20948.h"
#include "fpu.h"
#include "timer6.h"
#include "nrf24.h"
#include "MadgwickAHRS.h"
#include "dma.h"

// RF Channel for the Radio
#define RX_P0_CHANNEL 0

// Success/Fail light flashing time delays
#define FLASH_FAIL 100
#define FLASH_SUCCESS 250

// Maximum motor/control values
#define MAX_THROTTLE 90 // Out of 100
// #define MIN_THROTTLE 10  // Out of 100
#define MIN_ROLL -30  // Degrees
#define MAX_ROLL 30   // Degrees
#define MIN_PITCH -20 // Degrees
#define MAX_PITCH 20  // Degrees
#define MIN_YAW 0     // Degrees/sec
#define MAX_YAW 10    // Degrees/sec
#define MIN_INT16 -32768
#define MAX_INT16 32767

// Roll, Pitch, and Yaw gains for mixer
#define PITCH_GAIN 0.8f
#define ROLL_GAIN 0.8f
#define YAW_GAIN 0.15f

enum CONTROLLER_BUTTON
{
    BUTTON_A = 0,
    BUTTON_B = 1,
    BUTTON_X = 2,
    BUTTON_Y = 3,
    BUTTON_LB = 4,
    BUTTON_RB = 5,
    BUTTON_L3 = 9,
    BUTTON_R3 = 10
};

typedef struct
{
    float Kp;
    float Ki;
    float Kd;
    float error;
    float prev_error;
    float I;
} PID_Controller;

typedef struct
{
    uint8_t duty1;
    uint8_t duty2;
    uint8_t duty3;
    uint8_t duty4;
} DutyCycles;

enum RADIO_STATE
{
    TRANSMIT,
    RECEIVE
};

enum MOTOR_STATE
{
    OFF,
    IDLE,
    OPERATING
};

#endif // MAIN_H
