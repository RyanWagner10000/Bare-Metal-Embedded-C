# Bare-Metal-Embedded-C

Picture coming soon when it doesn't look like a rat's nest of wires :)

## Table of Contents

- [Description](#description)
- [Requirements](#requirements)
- [Flashing](#flashing-the-board)
- [Used Components](#external-components)
- [Peripheral Settings](#current-peripheral-settings)
- [Complimentary Filter](#complimentary-filter)

<br>

## Description

This repo is mainly for practicing bare-metal embedded C programming on the STM32F407G-DISC1 board and following the book: Bare-Metal Embedded C Programming by Israel Gbati

The goal is to make an autonomous drone, but if that doesn't happen it's okay. I'm just trying to learn as much as I can. Along that note, this repo does not use and CMSIS files or HAL libraries from STM. I figured that would degrade my learning experience and blow up the size of my code.

<br>

## Requirements
* GNU ARM Embedded Toolchain
* OpenOCD
* GCC for Windows or Linux
* Python 3.X

<br>

## Flashing the Board
1. Edit the .c/.h files accordingly
2. cd into the root folder of this project
3. Run the build.sh bash script to build the whole project
4. Run the flash.sh bash script to flash the compiled ELF file to the board. Make sure the board is plugged in to flash
5. If you want to read the USART output from the device
   1. Make sure the USART-to-USB board is wired and plugged in
   2. Launch a serial USB port reading application in another terminal

<br>

#### Special Instructions for Flashing on Windows
* If trying to compile on WSL, you must do the following steps so that the board is able to be seen by openOCD
  1. Launch a PowerShell window in admin mode
  2. Run the following line: `usbipd list`
  3. Note the ID of the board from the list
  4. Run the following line inserting the ID: `usbipd attach --busid <BUSID> --wsl`

<br>

## External Components

### ICM 20948 9-DOF IMU

##### Links

| [Overview](https://learn.adafruit.com/adafruit-tdk-invensense-icm-20948-9-dof-imu/overview) | [Datasheet](https://invensense.tdk.com/wp-content/uploads/2016/06/DS-000189-ICM-20948-v1.3.pdf) | [Pinout](https://learn.adafruit.com/adafruit-tdk-invensense-icm-20948-9-dof-imu/pinouts) |

##### Description

This is a very cheap IMU that probably isn't the best for a drone, but it's what I had lying around and it does the trick. Nonetheless, it has a 3-axis accelerometer, gyroscope, and magnetometer, as well as a temperature sensor if you choose to add that into your filter bias calculations.

##### Configuration

###### Accelerometer

The accelerometer is configured such that the sample rate is reduced by a factor of 10; the initial sample rate is 1.125 kHz, and I'm reducing it down to 112.5 Hz. I did this to make it easier to poll the module and get it's values for the complimentary filter.

The built-in digital low pass filter is enabled and set to 11.5Hz 3dB BW and 17.0Hz NBW, and the scale is set to &plusmn;2g.

The accelerometer also has a 32x averaging applied automatically on the module.

###### Gyroscope

The gyroscope is configured such that the sample rate is reduced by a factor of 5; the initial sample rate is 1.1 kHz, and I'm reducing it down to 220 Hz. I did this to make it easier to poll the module and get it's values for the complimentary filter.

The built-in digital low pass filter is enabled and set to 11.6Hz 3dB BW and 17.8Hz NBW, and the scale is set to &plusmn;250dps.

The gyro also has an 8x averaging applied automatically on the module.

###### Magnetometer

Coming soon!

###### Temperature

Coming soon!

---

### Motor Driving Board

##### Links

* [Coming Soon](https://google.com)

##### Description

Coming soon!

---


### Motors

##### Links

* [Coming Soon](https://google.com)

##### Description

Coming soon!

---


### RF Transceiver

##### Links

* [Coming Soon](https://google.com)

##### Description

Coming soon!

---


### Battery Pack

##### Links

* [Coming Soon](https://google.com)

##### Description

Coming soon!

---


### Power Controller

##### Links

* [Coming Soon](https://google.com)

##### Description

Coming soon!

---


## Current Peripheral Settings

### General Notes

* Uses base clock of 16 MHz for CPU and all buses
* The NVIC table is written in the general ARM Cortex-M4 chip, and is not tailored for this specific chip
  * Therefore, extra care was taken to only use the interrupts this chip has

### Timers

#### TIM2

This timer is used as the update frequency for the 9-DOF IMU and the complimentary filter.

It also has an interrupt triggered on its overflow such that it sets a global flag indicating to the Main loop that the sensor data is ready to be read and used in the complimentary filter.

#### TIM5

This timer is used as general 1ms timer for waiting/sleeping purposes. Mainly used when initializing things.

### LED's

#### Green, Blue, Red, and Orange

All of these LED's are initialized and are typically used to show success/failure modes. There may be combinations of these LED's in the future, but right now they operate individually.

* Green: Success of initializing all peripherals, and then a blink showing the updating of the complimentary filter
* Blue: To show a BusFault has happened
* Red: To show a HardFault has happened
* Orange: To show a UsageFault has happened

### USART

#### USART2

This peripheral uses GPIO pins PD5 & PD6 in alternate function mode to enable basic USART communication to my development machine. I have configured the protocol to operate at 9600 baud, but may increase in the future depending on frequency of messaging; this peripheral will be toggled off in the final deployment of the code as a computer will not be connected to it at all times. Therefore, USART is mainly used for debugging.

### SPI

#### SPI1

This peripheral uses GPIO pins PB3, PB4, PB5, and PB7 to communicate via SPI to the IMU module. If other devices need to communicate via SPI1 (and at the same frequency), then they will share the SDO, SDI, and SCL pins as the IMU module, but will need another GPIO pin for the CS of the new device. SPI1 is setup to operate at 1MHz, reduced from the 16MHz bus frequency; this was done to limit extra noise on the sensor.

## Complimentary Filter

The complimentary filter currently implemented uses the scaled values of the accelerometer and gyroscope (with static bias setting) to obtain a rough estimate of the orientation of the module in 3D space. The settings configured in the code shows a very smooth transition towards the roll/pitch the device is at. Therefore, this is **not** meant to be used on any fast moving drone at the moment. The current plan is to hone this complimentary filter to make it a little more responsive, and then transition to a Mahony or Madgwick filter and include the sensor temperature and gyroscope into the sensor fusion.

The complimentary filter operate as follows:

1. Get the raw XYZ values of the gyroscope
2. Scale the gyro readings based on the configuration of that sensor
3. Get the raw XYZ values of the accelerometer
4. Scale the accelerometer values based on the configuration of that sensor
5. Calculate the change in gyro x angular velocity using the update frequency
    1. Do so for the gyro y angular velocity too
6. Add the delta in x (roll) and y (pitch) to the global roll and pitch variables
7. Calculate the roll and pitch with the accelerometer values
8. Calculate the final roll and pitch angles by combining the gyro and accelerometer roll and pitchs values.
    1. A constant alpha of 0.9 biases towards using the gyro values and the inverse (0.1) towards the accelerometer values

<br>

<br>