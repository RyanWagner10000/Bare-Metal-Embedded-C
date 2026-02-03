/*
 * file: printing.c
 * description: my basic version of stdio.c
 * author: Ryan Wagner
 * date: December 28, 2025
 * notes:
 */

#include "printing.h"

/**
 * @brief Calculate the length of a character array
 *
 * @param s Character array
 *
 * @return Size of input character array
 */
uint32_t get_string_length(const char *s)
{
    uint32_t length = 0;

    while (s[length] != '\0')
    {
        length++;
    }

    return length;
}

/**
 * @brief Concatenate two character arrays into one
 *
 * @param str1 First character array
 * @param str2 Second character array
 * @param concat Resulting joining character array
 *
 * @return None
 *
 * @note Size of concat vairable must be of sufficient size to hold both strings or extra characters will be lost
 */
void str_concat(char *str1, char *str2, char *concat)
{
    uint32_t i = 0;
    uint32_t j = 0;

    while (str1[i] != '\0' && i < MAX_STRING_CONCAT)
    {
        concat[i] = str1[i];
        i++;
    }

    if (i == MAX_STRING_CONCAT - 1)
    {
        concat[i] = '\0';
        return;
    }

    /* Copy characters from the source string to the end of the destination string */
    while (str2[j] != '\0' && i < MAX_STRING_CONCAT)
    {
        concat[i] = str2[j];
        i++;
        j++;
    }

    /* Null-terminate the final concatenated string */
    concat[i] = '\0';

    return;
}

/**
 * @brief Reverse a string
 *
 * @param str Given charcter array to reverse
 * @param len Length of charcter array
 *
 * @return None
 */
void reverse(char *str, uint32_t len)
{
    uint32_t i = 0, j = len - 1;
    while (i < j)
    {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

/**
 * @brief Convert an integer to a character array
 *
 * @param n Integer to convert to string
 * @param buffer Buffer to store digits as characters
 *
 * @return Length of the converted string
 */
// Converts an integer 'n' to a string and stores it in 'buffer'
uint32_t int_to_str(int32_t n, char *buffer)
{
    uint32_t length = 0;
    uint32_t is_neg = 0;
    uint32_t num; // Use unsigned for the actual conversion

    if (n == 0)
    {
        buffer[length++] = '0';
        buffer[length] = '\0';
        return length;
    }

    if (n < 0)
    {
        is_neg = 1;
        num = (uint32_t)(-n); // Convert to unsigned after negating
    }
    else
    {
        num = (uint32_t)n;
    }

    while (num != 0)
    {
        uint32_t rem = num % 10;
        buffer[length++] = rem + '0';
        num = num / 10;
    }

    if (is_neg)
    {
        buffer[length++] = '-';
    }

    buffer[length] = '\0';
    reverse(buffer, length);

    return length;
}

/**
 * @brief Convert a float to a character array
 *
 * @param n Float to convert to string
 * @param buffer Buffer to store digits as characters
 * @param afterpoint Decimal point resolution of the float conversion
 *
 * @return None
 */
// Converts a float to a string in char array res[]
// afterpoint: number of digits to consider after the decimal point
void float_to_str(float n, char *buffer, uint32_t afterpoint)
{
    afterpoint++;

    // Handle negative numbers
    uint32_t index = 0;
    if (n < 0.0f)
    {
        buffer[index++] = '-';
        n = -n;
    }

    // Extract integer part as an actual integer
    uint32_t ipart = (uint32_t)n;

    // Extract fractional part
    float fpart = n - (float)ipart;

    // Convert integer part to string
    int32_t i = int_to_str(ipart, buffer + index);
    i += index;

    // Check for decimal part
    if (afterpoint != 0)
    {
        buffer[i++] = '.';

        // Get fractional digits
        for (uint32_t k = 0; k < afterpoint; k++)
        {
            fpart *= 10.0f;
            uint32_t digit = (uint32_t)fpart;
            buffer[i++] = '0' + digit;
            fpart -= (float)digit;
        }
        buffer[i] = '\0';
    }

    return;
}
