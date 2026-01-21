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
#include "printing.h"
#include "usart.h"

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

#define ACCEL_SMPLRT_DIV_1 (0x10)
#define ACCEL_SMPLRT_DIV_2 (0x11)
#define ACCEL_CONFIG (0x14)
#define ACCEL_CONFIG2 (0x15)

#define GYRO_SMPLRT_DIV (0x00)
#define GYRO_CONFIG1 (0x01)
#define GYRO_CONFIG2 (0x02)

// void initICM20948(void);
uint8_t initICM20948(void);
void printRegister(uint8_t address);
void getXYZ(uint8_t address, uint8_t *data);
void writeIsm20948(uint8_t address, uint8_t value);
uint8_t readRegister(uint8_t address);

#endif // ICM20948_H
