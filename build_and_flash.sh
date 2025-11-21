#!/bin/bash
# Create a new build folder and make the project

function try()
{
    [[ $- = *e* ]]; SAVED_OPT_E=$?
    set +e
}

function throw()
{
    exit $1
}

function catch()
{
    export exception_code=$?
    (( $SAVED_OPT_E )) && set +e
    return $exception_code
}

# Clear terminal
clear

# Remove build folder if it exists
rm -rdf build/

# Make new build folder, but don't error if it already exists
mkdir -p build

# cd into new build folder
cd build

OS_NAME=$(uname)

if [ "$OS_NAME" == "Linux" ]; then
    echo "Running on Linux."
    
    # Try to Make CMake project in build folder
    try
    (
        cmake ..
    )
    catch || {
        echo "'cmake ..' operation was unsuccessful."
        exit 1
    }

    # Try to Make the project
    try
    (
        cmake --build . -j4
    )
    catch || {
        echo "'cmake --build . -j4' operation was unsuccessful."
        exit 1
    }

    try
    (
        openocd -f ../scripts/stlink.cfg -f ../scripts/stm32f4x.cfg -c 'program firmware.elf verify reset exit'
    )
    catch || {
        echo "Launching openOCD was unsuccessful."
        exit 1
    }

elif [[ "$OS_NAME" == CYGWIN* || "$OS_NAME" == MINGW* ]]; then
    echo "Running on Windows (Cygwin or Git Bash)."
    
    # Try to Make CMake project in build folder
    try
    (
        cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi-gcc.cmake ..
    )
    catch || {
        echo "'cmake ..' operation was unsuccessful."
        exit 1
    }

    # Try to Make the project
    try
    (
        cmake --build . -j4
    )
    catch || {
        echo "'cmake --build . -j4' operation was unsuccessful."
        exit 1
    }

    try
    (
        OPENOCD_COMMAND="openocd -f ../scripts/stlink.cfg -f ../scripts/stm32f4x.cfg -c 'program firmware.elf verify reset exit'"
        powershell.exe -Command "Start-Process powershell -ArgumentList '-NoExit', \"& { $OPENOCD_COMMAND }\""
    )
    catch || {
        echo "Launching openOCD was unsuccessful."
        exit 1
    }

else
    echo "Unknown OS: $OS_NAME"
    exit 1
fi



echo " -- DONE! -- "

# Sources
# https://www.xmodulo.com/catch-handle-errors-bash.html