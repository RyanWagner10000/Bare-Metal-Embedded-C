/*
 * file: main.c
 * description: file that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#include "main.h"

/**
 * @brief Initiate all peripherals for the system
 *
 * @param None
 *
 * @return None
 *
 * @note Flashes the green LED 5 times upon success
 */
void initPeripherals(void)
{
    // Enable Floating Point Unit
    enableFaults();
    enableFPU();

    // Enable all the LED's
    initGreenLED();
    initOrangeLED();
    initRedLED();
    initBlueLED();

    // Enable all the timers
    initTimer2();
    initTimer5();
    initTimer6();
    initTimer8();

    // Enable the push button on the board
    initPushButton();

    // Enable USART for testing
    initUSART2();
    usartWriteString("Testing USART Connection!\n");

    // Enable SPI buses for IMU and Radio
    initSPI1();
    usartWriteString("SPI1 successfully initialized!\n");
    // initSPI2();
    // usartWriteString("SPI2 successfully initialized!\n");
    initSPI3();
    usartWriteString("SPI3 successfully initialized!\n");

    // Enable IMU
    initBerryIMU();
    usartWriteString("IMU successfully initialized!\n");

    // Enable Radio
    initRadio();
    usartWriteString("Radio successfully initialized!\n");
    // setRxMode(0);
    // usartWriteString("Rx Mode set successfully!\n");
    printRadioSettings();

    // Show success
    for (uint8_t i = 0; i < 2; i++)
    {
        onLED(GREEN_LED);
        for (uint32_t j = 0; j < 100000; j++)
            ;
        offLED(GREEN_LED);
        for (uint32_t j = 0; j < 100000; j++)
            ;
    }

    return;
}

/**
 * @brief Main forever while-loop
 *
 * @param None
 *
 * @return None
 */
int main(void)
{
    initPeripherals();

    // Get and set inital state
    uint32_t button_state = getButtonState();
    uint8_t run = 0;
    // int16_t imu1_xl_data[3];
    // int16_t imu2_xl_data[3];

    offLED(GREEN_LED);
    offLED(RED_LED);
    offLED(ORANGE_LED);
    offLED(BLUE_LED);

    volatile uint8_t temp2 = 0;
    temp2 = getWhoAmIxlgy(IMU1);
    usartWriteString("IMU1 Acc & Gyro ID: ");
    usartWriteNumber(temp2);
    usartWriteString("Expected ID: ");
    usartWriteNumber(0x6A);

    // volatile uint8_t temp2 = 0;
    // temp2 = 0;
    // temp2 = getWhoAmIxlgy(IMU2);
    // usartWriteString("IMU2 Acc & Gyro ID: ");
    // usartWriteNumber(temp2);
    // usartWriteString("Expected ID: ");
    // usartWriteNumber(0x6A);

    while (1)
    {
        // Check if the button has been set ON/OFF
        button_state = getButtonState();
        if (button_state)
        {
            run++;
            run = run & 0x01;
            offLED(GREEN_LED);
            delayMillisecond(150);
        }

        // Flag to indicate update of the IMU complimetary filter
        if (getImuFlag())
        {
            // Reset check flag
            setImuFlag(0);

            // Complimentary filter
            // logRawAccelData(IMU1, imu1_xl_data);
            // logRawAccelData(IMU2, imu2_xl_data);
        }

        if (run)
        {
            temp2 = getWhoAmIxlgy(IMU1);
            usartWriteString("\nIMU1 Acc & Gyro ID: ");
            usartWriteNumber(temp2);
            // temp2 = getWhoAmIxlgy(IMU2);
            // usartWriteString("IMU2 Acc & Gyro ID: ");
            // usartWriteNumber(temp2);

            // Log IMU 1: Accel to screen
            // logRawAccelData(IMU2, imu2_xl_data);

            // Log IMU 1: Gyro to screen
            // Log IMU 1: Mag to screen

            // Log IMU 2: Accel to screen
            // Log IMU 2: Gyro to screen
            // Log IMU 2: Mag to screen

            // Set speed motor output
            // setDuty(CH1, 30);
            // setDuty(CH2, 30);
            // setDuty(CH3, 30);
            // setDuty(CH4, 30);

            // Toggle LED to show it's working
            toggleLED(GREEN_LED);
            delayMillisecond(50);
        }
        // else
        // {
        //     // Set speed motor output
        //     setDuty(CH1, 0);
        //     setDuty(CH2, 0);
        //     setDuty(CH3, 0);
        //     setDuty(CH4, 0);
        //     delayMillisecond(10);
        // }
    }
}
