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
#include "timer.h"

// General registers
#define RESET_REGISTER (0x00)
#define ACCEL_GYRO_ID (0xEA)
#define MAG_ID (0x09)
#define MAG_ADDRESS (0x0C)
#define READ_OPERATION (0x80)
#define MULTI_BYTE_ENABLE (0x40)
#define USER_CTRL (0x03)
#define PWR_MGMT_1 (0x06)
#define PWR_MGMT_2 (0x07)
#define ACCEL_DATA (0x2D)
#define GYRO_DATA (0x33)
#define REG_BANK_SEL (0x7F)

// Accel registers
#define ACCEL_SMPLRT_DIV_1 (0x10)
#define ACCEL_SMPLRT_DIV_2 (0x11)
#define ACCEL_CONFIG (0x14)
#define ACCEL_CONFIG2 (0x15)

// Gyro registers
#define GYRO_SMPLRT_DIV (0x00)
#define GYRO_CONFIG1 (0x01)
#define GYRO_CONFIG2 (0x02)

// Mag communication registers
#define USER_CNTL (0x03)
#define I2C_MST_CTRL (0x01)
#define I2C_SLV0_ADDR (0x03)
#define I2C_SLV0_REG (0x04)
#define I2C_SLV0_CTRL (0x05)
#define I2C_SLV0_DO (0x06)
#define EXT_SLV_SENS_DATA_00 (0x3B)

// Mag registers
#define MAG_CONFIG (0x31)
#define MAG_STATUS (0x10)
#define MAG_DATA (0x11)
#define CNTL2 (0x31)
#define CNTL3 (0x32)

void initICM20948(void);
void printRegister(uint8_t address);
void getXYZ(uint8_t address, uint8_t *data);
void writeIsm20948(uint8_t address, uint8_t value);
uint8_t readRegister(uint8_t address);

void logRawMagnetometer(void);
void logRawAccelerometer(void);
void logRawGyroscope(void);

#endif // ICM20948_H
