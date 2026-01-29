/*
 * file: fpu.c
 * description: floating point unit configuration
 * author: Ryan Wagner
 * date: January 9, 2026
 * notes:
 */

#include "fpu.h"

void enable_fpu(void)
{
    // Read-modify-write to enable CP10 and CP11
    uint32_t cpacr = CPACR;
    cpacr &= ~(0xF << 20); // Clear bits first
    cpacr |= (0xF << 20);  // Set both CP10 and CP11 to full access (0b11)
    CPACR = cpacr;

    // Barriers
    __asm__ volatile("dsb" ::: "memory");
    // "Wait for the CPACR write to actually complete"
    __asm__ volatile("isb" ::: "memory");
    // "Flush the pipeline and re-fetch all instructions, now that the FPU is enabled"

    return;
}

void enable_faults(void)
{
    // Enable UsageFault, BusFault, and MemManageFault handlers
    SCB_SHCSR |= (1 << 18) | (1 << 17) | (1 << 16);
}
