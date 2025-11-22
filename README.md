# Bare-Metal-Embedded-C
This repo is mainly for practicing bare metal embedded C programming on the STM32F407G-DISC1 board and following the book: Bare-Metal Embedded C Programming by Israel Gbati

## Requirements
* openocd
* GNU ARM Embedded Toolchain
* GCC for Windows or Linux

## Flashing the Chip
1. Edit the .c/.h files accordingly
2. cd into the root folder of this project
3. Run the build_and_flash.sh bash script

### Notes
* If trying to compile on WSL, you must do the following steps so that the board is able to be seen by openOCD
  1. Launch a PowerShell window in admin mode
  2. Run the following line: `usbipd list`
  3. Note the ID of the board from the list
  4. Run the following line inserting the ID: `usbipd attach --busid <BUSID> --wsl`