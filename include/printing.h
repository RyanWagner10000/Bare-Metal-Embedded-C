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

#define MAX_STRING_CONCAT (1024)
#define MAX_FLOAT_STRING (128)

uint32_t get_string_length(const char *s);
uint32_t int_to_str(int32_t n, char *buffer);
void float_to_string(float n, char *res, uint32_t afterpoint);
void str_concat(char *str1, char *str2, char *concat);

#endif // PRINTING_H
