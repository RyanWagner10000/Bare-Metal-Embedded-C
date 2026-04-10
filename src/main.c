/*
 * file: main.c
 * description: file that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#include "main.h"

uint8_t packets_received = 0;

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
    usartWriteString("IMU1 Acc & Gyro ID: ");
    usartWriteNumber((int32_t)getWhoAmIxlgy(IMU1));
    usartWriteChar('\n');
    // usartWriteString("IMU1 Mag ID: ");
    // usartWriteNumber((int32_t)getWhoAmIMag(IMU1));
    // usartWriteChar('\n');
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

/**
 * @brief Clamp a value based on a min and max
 *
 * @param value Value to clamp between low and high
 * @param low Low/minimum value to clamp to
 * @param high High/Maximum value to clamp to
 *
 * @return Clamped float value
 */
float clamp(float value, float low, float high)
{
    return value < low ? low : (value > high ? high : value);
}

/**
 * @brief Calculate the duty cycles for the motors based on recent packet info
 *
 * @param packet RadioPacket object with information
 * @param duty_cycles DutyCycles object to set duty cycles
 *
 * @return None
 */
void calculateMotorDuty(RadioPacket packet, DutyCycles *duty_cycles)
{
    // Normalize values: throttle [0, 1], roll, pitch, yaw [-1, 1]
    float norm_throttle = (float)(packet.throttle) / (float)(MAX_INT16);                    // [0, 1]
    float norm_pitch = (((float)packet.pitch - (float)MIN_INT16) / (32767.5)) - 1.0;        // [-1,1]
    float norm_roll = ((((float)packet.roll * -1.0) - (float)MIN_INT16) / (32767.5)) - 1.0; // [-1,1]
    float norm_yaw = (((float)packet.yaw - (float)MIN_INT16) / (32767.5)) - 1.0;            // [-1,1]

    // Mix the values
    float m1 = norm_throttle + PITCH_GAIN * norm_pitch - ROLL_GAIN * norm_roll - YAW_GAIN * norm_yaw;
    float m2 = norm_throttle + PITCH_GAIN * norm_pitch + ROLL_GAIN * norm_roll + YAW_GAIN * norm_yaw;
    float m3 = norm_throttle - PITCH_GAIN * norm_pitch - ROLL_GAIN * norm_roll + YAW_GAIN * norm_yaw;
    float m4 = norm_throttle - PITCH_GAIN * norm_pitch + ROLL_GAIN * norm_roll - YAW_GAIN * norm_yaw;

    // Clamp mixed values to PWM range [0 - 90]
    duty_cycles->duty1 = (uint8_t)(clamp(m1, 0.0f, 0.9f) * 100.0f);
    duty_cycles->duty2 = (uint8_t)(clamp(m2, 0.0f, 0.9f) * 100.0f);
    duty_cycles->duty3 = (uint8_t)(clamp(m3, 0.0f, 0.9f) * 100.0f);
    duty_cycles->duty4 = (uint8_t)(clamp(m4, 0.0f, 0.9f) * 100.0f);

    return;
}

/**
 * @brief Print the number of packets received over one second
 *
 * @param None
 *
 * @return None
 */
void logPacketDrop(void)
{
    usartWriteString("Received Packets: ");
    usartWriteNumber(packets_received);
    usartWriteChar('\n');
    packets_received = 0;
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
    initModules();

    // Get and set inital state
    // uint32_t button_state = getButtonState();

    offLED(GREEN_LED);
    offLED(RED_LED);
    offLED(ORANGE_LED);
    offLED(BLUE_LED);

    // Struct to hold TxRx data from ground-station
    RadioPacket packet = {0, 0, 0, 0, 0, 0, 255, 0};
    DutyCycles duty_cycles = {0, 0, 0, 0};
    uint8_t txrx = 1; // 0 for tx, 1 for rx

    // State initializations
    // enum IMU_STATE imu_state = IDLE;
    enum RADIO_STATE radio_state = RECEIVE;
    // enum FILTER_STATE filter_state = IDLE;
    enum MOTOR_STATE motor_state = OFF;

    while (1)
    {
        // Handle radio Tx/Rx
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

                // Incrememnt packets received to be calculated
                // packets_received++;
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

        if (getOneSecFlag())
        {
            // Reset flag
            setOneSecFlag(0);

            // Print reception rate
            // logPacketDrop();
        }

        if (getImuFlag())
        {
            // Reset flag
            setImuFlag(0);

            // Read data from IMU
            // logRawAccelData(IMU1);
            // logRawGyroData(IMU1);
            // logRawMagData(IMU1);
        }

        if (getPWMFlag())
        {
            // Reset flag
            setPWMFlag(0);

            switch (motor_state)
            {
            case OFF:
            {
                // Check for power-up/down command
                if (packet.button == 0)
                {
                    // User has initiated power-up
                    motor_state = IDLE;
                }
                // Set speed motor output
                setDuty(CH1, 0);
                setDuty(CH2, 0);
                setDuty(CH3, 0);
                setDuty(CH4, 0);
                break;
            }
            case IDLE:
            {
                // Set speed motor output
                setDuty(CH1, 5);
                setDuty(CH2, 5);
                setDuty(CH3, 5);
                setDuty(CH4, 5);

                motor_state = OPERATING;
                break;
            }
            case OPERATING:
            {
                // Check for power-up/down command
                if (motor_state == OPERATING && packet.button == 1)
                {
                    // User has initiated a power-down
                    motor_state = OFF;
                }

                // Calculate motor duty cycles from packet
                calculateMotorDuty(packet, &duty_cycles);

                // usartWriteString("  1: ");
                // usartWriteNumber((int32_t)duty_cycles.duty1);
                // usartWriteString("  2: ");
                // usartWriteNumber((int32_t)duty_cycles.duty2);
                // usartWriteString("  3: ");
                // usartWriteNumber((int32_t)duty_cycles.duty3);
                // usartWriteString("  4: ");
                // usartWriteNumber((int32_t)duty_cycles.duty4);
                // usartWriteString("    \r");

                // Set speed motor output
                setDuty(CH1, duty_cycles.duty1);
                setDuty(CH3, duty_cycles.duty2);
                setDuty(CH2, duty_cycles.duty3);
                setDuty(CH4, duty_cycles.duty4);
                break;
            }
            default:
            {
                motor_state = OFF;
                break;
            }
            }
        }
    }
}
