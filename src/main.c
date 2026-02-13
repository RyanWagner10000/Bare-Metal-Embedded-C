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
    enableFaults();
    enableFPU();

    initGreenLED();
    initOrangeLED();
    initRedLED();
    initBlueLED();

    initTimer2();
    initTimer5();

    initPushButton();

    initUSART2();

    initSPI();

    initICM20948();

    // Show success
    for (uint8_t i = 0; i < 5; i++)
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

    offLED(GREEN_LED);

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
            calculateAttitude(0.004);
        }

        if (run)
        {
            // Print attitude to the screen
            // logAttitude();

            logRawMagnetometer();
            // logRawAccelerometer();
            // logRawGyroscope();

            // Toggle LED to show it's working
            toggleLED(GREEN_LED);
        }
    }
}
