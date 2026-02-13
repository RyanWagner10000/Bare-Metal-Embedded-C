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

#define MAX_STRING_CONCAT (512)
#define MAX_FLOAT_STRING (64)
#define MAX_INT_STRING (16)

uint32_t getStringLength(const char *s);
uint32_t intToStr(int32_t n, char *buffer);
void floatToStr(float n, char *res, uint32_t afterpoint);
void strConcat(char *str1, char *str2, char *concat);

#endif // PRINTING_H
