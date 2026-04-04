/*
 * file: main.c
 * description: file that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#include "main.h"

/**
 * @brief Flash all LEDs
 *
 * @param delay Delay in milliseconds between flashes
 *
 * @return None
 */
void flashAllLED(uint32_t delay)
{
    onLED(RED_LED);
    onLED(ORANGE_LED);
    onLED(BLUE_LED);
    onLED(GREEN_LED);

    delayMillisecond(delay);

    offLED(RED_LED);
    offLED(ORANGE_LED);
    offLED(BLUE_LED);
    offLED(GREEN_LED);

    delayMillisecond(delay);

    return;
}

/**
 * @brief Initiate all peripherals for the system
 *
 * @param None
 *
 * @return None
 *
 * @note Flashes all LED's 1 time upon success
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

    // Show success
    flashAllLED(FLASH_SUCCESS);

    return;
}

/**
 * @brief Initiate all peripherals for the system
 *
 * @param None
 *
 * @return None
 *
 * @note Flashes all LED's 1 time upon success, flashes faster continuously if failed
 */
void initModules(void)
{
    // Enable IMU
    initBerryIMU();
    usartWriteString("\nIMU successfully initialized!\n");

    // Enable Radio
    if (initRadio(RX_P0_CHANNEL))
    {
        usartWriteString("\nRadio successfully initialized!\n");

        // Now set for Rx mode
        if (setRxMode())
            usartWriteString("Rx Mode set successfully!\n");
        else
        {
            usartWriteString("Rx Mode NOT set successfully.\n");
            while (1)
            {
                flashAllLED(FLASH_FAIL);
            }
        }
    }
    else
    {
        usartWriteString("Radio initialization unsuccessful.\n");
        while (1)
        {
            flashAllLED(FLASH_FAIL);
        }
    }

    printRadioSettings();

    // Show success
    flashAllLED(FLASH_SUCCESS);
}


void calculateMotorDuty(RadioPacket packet, DutyCycles *duty_cycles)
{
    // Set the duty cycles for the motors based on recent packet info
    duty_cycles->duty1 = 0;
    duty_cycles->duty2 = 0;
    duty_cycles->duty3 = 0;
    duty_cycles->duty4 = 0;
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
    initModules();

    // Get and set inital state
    // uint32_t button_state = getButtonState();

    offLED(GREEN_LED);
    offLED(RED_LED);
    offLED(ORANGE_LED);
    offLED(BLUE_LED);

    // Struct to hold TxRx data from ground-station
    RadioPacket packet;
    DutyCycles duty_cycles = {0, 0, 0, 0};
    uint8_t txrx = 1; // 0 for tx, 1 for rx

    // State initializations
    // enum IMU_STATE imu_state = IDLE;
    enum RADIO_STATE radio_state = RECEIVE;
    // enum FILTER_STATE filter_state = IDLE;
    // enum MOTOR_STATE motor_state = OFF;

    uint8_t temp = getWhoAmIxlgy(IMU1);
    usartWriteString("IMU1 Acc & Gyro ID: ");
    usartWriteNumber((int32_t)temp);

    while (1)
    {
        switch (radio_state)
        {
        case RECEIVE:
        {
            // Check for any incoming packets from ground station
            if (dataAvailable())
            {
                // Check if Rx Mode already set
                if (!txrx)
                {
                    // Set Rx Mode
                    if (!setRxMode())
                        break;
                    txrx = 1;
                }

                // Read Rx data, print if available
                readRadio(&packet, P0_PACKET_SIZE);

                // Print packet for confirmation
                // printPacket(packet);
                calculateMotorDuty(packet, &duty_cycles);
            }
            break;
        }
        case TRANSMIT:
        {
            // Check if Tx Mode already set
            if (txrx)
            {
                // Set Tx Mode
                if (!setTxMode())
                    continue;
                txrx = 0;
            }

            // Check that the Tx FIFO isn't full
            if (txFIFOFull())
                continue;

            // Transmit data
            uint8_t *data = (uint8_t *)&packet;
            transmitRadio(data, P0_PACKET_SIZE);

            // Set back to Rx mode
            radio_state = RECEIVE;
            break;
        }
        default:
        {
            radio_state = RECEIVE;
            break;
        }
        }

        // switch (motor_state)
        // {
        // case OFF:
        // {
        //     // Set speed motor output
        //     setDuty(CH1, 0);
        //     setDuty(CH2, 0);
        //     setDuty(CH3, 0);
        //     setDuty(CH4, 0);
        //     break;
        // }
        // case IDLE:
        // {
        //     // Set speed motor output
        //     setDuty(CH1, 20);
        //     setDuty(CH2, 20);
        //     setDuty(CH3, 20);
        //     setDuty(CH4, 20);
        //     break;
        // }
        // case OPERATING:
        // {
        //     // Set speed motor output
        //     // setDuty(CH1, 0);
        //     // setDuty(CH2, 0);
        //     // setDuty(CH3, 0);
        //     // setDuty(CH4, 0);
        //     break;
        // }
        // default:
        // {
        //     break;
        // }
        // }
    }
}
