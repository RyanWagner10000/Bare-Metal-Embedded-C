/*
 * file: printing.c
 * description: my basic version of stdio.c
 * author: Ryan Wagner
 * date: December 28, 2025
 * notes:
 */

#include "printing.h"

uint32_t get_string_length(const char *s)
{
    uint32_t length = 0;

    while (s[length] != '\0')
    {
        length++;
    }

    return length;
}

// string concatenation
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

// Reverse a string in place
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

// Converts an integer 'n' to a string and stores it in 'buffer'
// Returns the length of the string
uint32_t int_to_str(int32_t n, char *buffer)
{
    uint32_t i = 0;
    uint32_t is_neg = 0;
    uint32_t num; // Use unsigned for the actual conversion

    if (n == 0)
    {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return i;
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
        buffer[i++] = rem + '0';
        num = num / 10;
    }

    if (is_neg)
    {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';
    reverse(buffer, i);

    return i;
}

// Converts a float to a string in char array res[]
// afterpoint: number of digits to consider after the decimal point
void float_to_string(float n, char *res, uint32_t afterpoint)
{
    afterpoint++;

    // Handle negative numbers
    uint32_t index = 0;
    if (n < 0.0f)
    {
        res[index++] = '-';
        n = -n;
    }

    // Extract integer part as an actual integer
    uint32_t ipart = (uint32_t)n;

    // Extract fractional part
    float fpart = n - (float)ipart;

    // Convert integer part to string
    int32_t i = int_to_str(ipart, res + index);
    i += index;

    // Check for decimal part
    if (afterpoint != 0)
    {
        res[i++] = '.';

        // Get fractional digits
        for (uint32_t k = 0; k < afterpoint; k++)
        {
            fpart *= 10.0f;
            uint32_t digit = (uint32_t)fpart;
            res[i++] = '0' + digit;
            fpart -= (float)digit;
        }
        res[i] = '\0';
    }

    return;
}
