# Bare-Metal-Embedded-C
This repo is mainly for practicing bare metal embedded C programming on the STM32F407G-DISC1 board and following the book: Bare-Metal Embedded C Programming by Israel Gbati

## Requirements
* GNU ARM Embedded Toolchain
* GCC for Windows or Linux
* Python 3.X

## Flashing the Chip
1. Edit the .c/.h files accordingly
2. cd into the root folder of this project
3. Run the build.sh bash script to build the whole project
4. Run the flash.sh bash script to flash the compiled ELF file to the board. Make sure the board is plugged in to flash
5. If you want to read the USART output from the device, make sure the USART board is wired and plugged in

### Notes
* If trying to compile on WSL, you must do the following steps so that the board is able to be seen by openOCD
  1. Launch a PowerShell window in admin mode
  2. Run the following line: `usbipd list`
  3. Note the ID of the board from the list
  4. Run the following line inserting the ID: `usbipd attach --busid <BUSID> --wsl`