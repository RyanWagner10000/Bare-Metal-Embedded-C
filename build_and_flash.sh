#!/bin/bash
# Create a new build folder and make the project

CURR_DIR=$(pwd)
INTERFACE_CFG="$CURR_DIR/scripts/stlink.cfg"
TARGET_CFG="$CURR_DIR/scripts/stm32f4x.cfg"

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

try
(
    # Clear terminal
    clear

    # Remove build folder if it exists
    rm -rdf build/

    # Make new build folder, but don't error if it already exists
    mkdir -p build

    echo ""
    echo "Cleared and remade /build directory."
    echo ""
)
catch || {
    echo "Clearing and re-making build dir was unsuccessful."
}

# cd into new build folder
cd build/

# Try to Make CMake project in build folder
try
(
    echo ""
    echo " -- Making CMake Project -- "
    echo ""
    cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi-gcc.cmake ..
)
catch || {
    echo "Project Making operation was unsuccessful."
    exit 1
}

# Try to Make the project
try
(
    echo ""
    echo " -- Building CMake Project -- "
    echo ""
    cmake --build . -j4
)
catch || {
    echo "Project Building operation was unsuccessful."
    exit 1
}

OS_NAME=$(uname)

if [ "$OS_NAME" == "Linux" ]; then

    try
    (
        echo ""
        echo " -- Launching openOCD -- "
        echo ""
        openocd -f "$INTERFACE_CFG" -f "$TARGET_CFG" -c 'program firmware.elf verify reset exit'
    )
    catch || {
        echo "Launching openOCD was unsuccessful."
        exit 1
    }

elif [[ "$OS_NAME" == CYGWIN* || "$OS_NAME" == MINGW* ]]; then
    
    try
    (
        echo ""
        echo " -- Launching openOCD -- "
        echo ""
        OPENOCD_COMMAND="openocd -f $INTERFACE_CFG -f $TARGET_CFG -c 'program firmware.elf verify reset exit'"
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