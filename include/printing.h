/*
 * file: printing.h
 * description: header file for PRINTING peripheral structure
 * author: Ryan Wagner
 * date: December 5, 2025
 * notes:
 */

#ifndef PRINTING_H
#define PRINTING_H

#include <stdint.h>
#include "usart.h"

#define MAX_STRING_CONCAT (512)
#define MAX_FLOAT_STRING (64)
#define MAX_INT_STRING (16)
#define MAX_BIN_STRING (64)
#define MAX_HEX_STRING (8)

enum printFormat {
    DECIMAL,
    HEX,
    BINARY
};

uint32_t getStringLength(const char *s);
uint32_t intToStr(int32_t n, char *buffer);
void floatToStr(float n, char *res, uint32_t afterpoint);
void strConcat(char *str1, char *str2, char *concat);
void printRegister(uint8_t address, uint32_t data, uint8_t format);

#endif // PRINTING_H
