/*
 * file: ICM20948.h
 * description: header file for 9 DOF IMU
 * author: Ryan Wagner
 * date: December 18, 2025
 * notes:
 */

#ifndef ICM20948_H
#define ICM20948_H

#include <stdint.h>
#include "spi.h"

#define RESET_REGISTER (0x00)
#define ID (0xEA)
#define READ_OPERATION (0x80)
#define MULTI_BYTE_ENABLE (0x40)

#define USER_CTRL (0x03)
#define PWR_MGMT_1 (0x06)
#define PWR_MGMT_2 (0x07)
#define ACCEL_DATA (0x2D)
#define GYRO_DATA (0x33)
#define MAG_DATA (0x11)
#define REG_BANK_SEL (0x7F)
#define ACCEL_CONFIG (0x14)
#define ACCEL_CONFIG2 (0x15)

// void initICM20948(void);
uint8_t initICM20948(void);

void readAccel(uint8_t address, uint8_t *data);
void readGyro(uint8_t address, uint8_t *data);
void writeIsm20948(uint8_t address, uint8_t value);
uint8_t readRegister(uint8_t address);

#endif // ICM20948_H
