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

# MCU flags - put EVERYTHING here for the target
target_compile_options(firmware.elf PRIVATE
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16       # ← Add this!
    -mfloat-abi=hard        # ← Add this!
    -std=gnu11
    -Wall
    -Wextra
)

target_link_options(firmware.elf PRIVATE
    -T${CMAKE_SOURCE_DIR}/linker/stm32_ls.ld
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16       # ← Add this!
    -mfloat-abi=hard        # ← Add this!
    -nostdlib
    --specs=nosys.specs     # ← Good to add this too
)

target_link_libraries(firmware.elf gcc m)  # ← Add 'm' for math library