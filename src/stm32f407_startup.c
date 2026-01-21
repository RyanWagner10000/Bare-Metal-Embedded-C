#include <stdint.h>
#include "gpio.h"

extern uint32_t _estack;
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void Default_Handler(void);
void Reset_Handler(void);
void HardFault_Handler(void);
void UsageFault_Handler(void);
void BusFault_Handler(void);
int main();

void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));

uint32_t vector_tbl[] __attribute__((section(".isr_vector_tbl"))) = {
    (uint32_t)&_estack,            // 0: Initial Stack Pointer
    (uint32_t)&Reset_Handler,      // 1: Reset
    (uint32_t)&NMI_Handler,        // 2: NMI
    (uint32_t)&HardFault_Handler,  // 3: HardFault - DON'T COMMENT THIS OUT!
    (uint32_t)&MemManage_Handler,  // 4: MemManage
    (uint32_t)&BusFault_Handler,   // 5: BusFault
    (uint32_t)&UsageFault_Handler, // 6: UsageFault
    0,                             // 7: Reserved
    0,                             // 8: Reserved
    0,                             // 9: Reserved
    0,                             // 10: Reserved
    (uint32_t)&Default_Handler,    // 11: SVCall
    (uint32_t)&Default_Handler,    // 12: Debug Monitor
    0,                             // 13: Reserved
    (uint32_t)&Default_Handler,    // 14: PendSV
    (uint32_t)&Default_Handler,    // 15: SysTick
};

void HardFault_Handler(void)
{
    initRedLED();
    while (1)
    {
        toggleLED(RED_LED);
        for (uint32_t i = 0; i < 500000; i++)
            ;
    }
}

void BusFault_Handler(void)
{
    initRedLED();
    while (1)
    {
        toggleLED(RED_LED);
        for (uint32_t i = 0; i < 500000; i++)
            ;
    }
}

void UsageFault_Handler(void)
{
    initRedLED();
    while (1)
    {
        toggleLED(RED_LED);
        for (uint32_t i = 0; i < 500000; i++)
            ;
    }
}

void Default_Handler(void)
{
    while (1)
    {
    }
}

void Reset_Handler(void)
{
    // Calculate the sizes of the .data and .bss sections
    uint32_t data_mem_size = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint32_t bss_mem_size = (uint32_t)&_ebss - (uint32_t)&_sbss;

    /* Convert byte sizes to 32-bit word counts to match uint32_t* copies (4 == sizeof(uint32_t)) */
    data_mem_size /= 4;
    bss_mem_size /= 4;

    // Initialize pointers to the source and destination of the .data section
    uint32_t *p_src_mem = (uint32_t *)&_etext;
    uint32_t *p_dest_mem = (uint32_t *)&_sdata;

    // Copy .data section from FLASH to SRAM
    for (uint32_t i = 0; i < data_mem_size; i++)
    {
        *p_dest_mem++ = *p_src_mem++;
    }
    // Initialize the .bss section to zero in SRAM
    p_dest_mem = (uint32_t *)&_sbss;

    for (uint32_t i = 0; i < bss_mem_size; i++)
    {
        // Set bss section to zero
        *p_dest_mem++ = 0;
    }

    // Call the application's main function
    main();
}
