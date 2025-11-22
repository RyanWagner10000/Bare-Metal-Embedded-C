cmake_minimum_required(VERSION 3.10)
project(stm32f4_project C ASM)

set(CMAKE_C_STANDARD 11)

# Include directory
include_directories(include)

# Source files
file(GLOB SRC_FILES
    src/*.c
    src/drivers/*.c
)

add_executable(firmware.elf ${SRC_FILES})

target_include_directories(firmware.elf PRIVATE include include/drivers)

# MCU flags
target_compile_options(firmware.elf PRIVATE
    -c
    -mcpu=cortex-m4
    -mthumb
    -std=gnu11
)

target_link_options(firmware.elf PRIVATE
    -T${CMAKE_SOURCE_DIR}/linker/stm32_ls.ld
    -nostdlib
)