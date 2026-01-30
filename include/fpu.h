/*
 * file: fpu.h
 * description: header file to enable and configure FPU on CPU
 * author: Ryan Wagner
 * date: January 9, 2026
 * notes:
 */

#ifndef FPU_H
#define FPU_H

#include <stdint.h>

#define SCB_SHCSR (*((volatile uint32_t *)0xE000ED24))

#define SCB_FP_EXTENSION (0xE000ED00)
#define CPACR_OFFSET (0x88)
#define FPCCR_OFFSET (0xF34)
#define FPCAR_OFFSET (0xF38)
#define FPDSCR_OFFSET (0xF3C)

// #define CPACR (*((volatile uint32_t *)SCB_FP_EXTENSION + CPACR_OFFSET))
#define CPACR (*((volatile uint32_t *)0xE000ED88))
// #define FPCCR (*((volatile uint32_t *)SCB_FP_EXTENSION + FPCCR_OFFSET))
// #define FPCAR (*((volatile uint32_t *)SCB_FP_EXTENSION + FPCAR_OFFSET))
// #define FPDSCR (*((volatile uint32_t *)SCB_FP_EXTENSION + FPDSCR_OFFSET))

void enable_fpu(void);
void enable_faults(void);

#endif // FPU_H
