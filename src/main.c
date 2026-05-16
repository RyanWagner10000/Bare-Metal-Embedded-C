/*
 * file: main.c
 * description: file that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#include "main.h"

#define RAD (0.017453293f)
#define dt (0.004f) // 250 Hz dt in seconds
uint8_t packets_received = 0;
Euler angles = {0.0f, 0.0f, 0.0f};
PID_Controller roll_pid = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0};

PID_Controller pitch_pid = {3.0f, 0.01f, 0.001f, 0.0f, 0.0f, 0};

PID_Controller yaw_pid = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0};

uint8_t MIN_THROTTLE = 40;

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
 * @note Flashes all LED's 1 time upon success, flashes faster continuously if failed
 */
void initModules(void)
{
    // Enable IMU
    if (initIMU(IMU2))
    {
        uint8_t temp_id = getAccelGyroID(IMU2);
        if (temp_id == XL_GYRO_ADDRESS)
        {
            usartWriteString("\nIMU successfully initialized and communicating!\n");
        }
        else
        {
            usartWriteString("IMU ID [");
            usartWriteNumber((uint32_t)temp_id);
            usartWriteString("] not recognized.\n");
            while (1)
            {
                flashAllLED(FLASH_FAIL);
            }
        }
    }
    else
    {
        usartWriteString("IMU initialization unsuccessful.\n");
        while (1)
        {
            flashAllLED(FLASH_FAIL);
        }
    }

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
    // Enable USART for testing
    initUSART2();
    usartWriteString("Testing USART Connection!\n");

    // Enable Floating Point Unit
    enableFaults();
    enableFPU();
    usartWriteString("FPU sucessfully initializaed!\n");

    // Enable all the LED's
    initGreenLED();
    initOrangeLED();
    initRedLED();
    initBlueLED();
    usartWriteString("LED's sucessfully initializaed!\n");

    // Enable all the timers
    initTimer2();
    usartWriteString("Timer2 sucessfully initializaed!\n");
    initTimer5();
    usartWriteString("Timer5 sucessfully initializaed!\n");
    initTimer6();
    usartWriteString("Timer6 sucessfully initializaed!\n");
    initTimer8();
    usartWriteString("Timer8 sucessfully initializaed!\n");

    // Enable the push button on the board
    // initPushButton();

    // Enable SPI buses for IMU and Radio
    // initSPI1();
    // usartWriteString("SPI1 successfully initialized!\n");
    initSPI2();
    usartWriteString("SPI2 successfully initialized!\n");
    initSPI3();
    usartWriteString("SPI3 successfully initialized!\n");

    // Init all modules
    initModules();

    // Enable DMA
    initDMA();
    usartWriteString("DMA sucessfully initialized!\n");
    // initDMA_SPI1();
    // usartWriteString("DMA for SPI1 sucessfully initialized!\n");
    initDMA_SPI2();
    usartWriteString("DMA for SPI2 sucessfully initialized!\n");

    // Show success
    flashAllLED(FLASH_SUCCESS);

    return;
}

float updatePID(PID_Controller *pid, float setpoint, float actual)
{
    pid->prev_error = pid->error;
    pid->error = setpoint - actual;

    float P = pid->Kp * pid->error;
    pid->I += pid->Ki * pid->error * dt;
    float D = pid->Kd * (pid->error - pid->prev_error) / dt;

    return P + pid->I + D;
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
    float norm_throttle = ((float)packet.throttle - (float)MIN_INT16) / 65535.0f;     // [0, 1]
    float norm_pitch = ((float)(-packet.pitch - (float)MIN_INT16) / (32767.5)) - 1.0; // [-1,1]
    float norm_roll = ((float)(-packet.roll - (float)MIN_INT16) / (32767.5)) - 1.0;   // [-1,1]
    float norm_yaw = ((float)(packet.yaw - (float)MIN_INT16) / (32767.5)) - 1.0;      // [-1,1]
    norm_throttle *= MAX_THROTTLE;
    norm_pitch *= MAX_PITCH;
    norm_roll *= MAX_ROLL;
    norm_yaw *= MAX_YAW;

    // usartWriteString("  nt: ");
    // usartWriteNumber((int32_t)norm_throttle);
    // usartWriteString("  nr: ");
    // usartWriteNumber((int32_t)norm_roll * 100.0f);
    // usartWriteString("  np: ");
    // usartWriteNumber((int32_t)norm_pitch * 100.0f);
    // usartWriteString("  ny: ");
    // usartWriteNumber((int32_t)norm_yaw * 100.0f);
    // usartWriteString("    \r");

    // Upadte PID controllers
    float roll_correction = updatePID(&roll_pid, norm_roll, angles.x); // Roll
    // if (roll_correction > -1 && roll_correction < 1)
    // {
    //     roll_correction = 0;
    // }
    float pitch_correction = updatePID(&pitch_pid, norm_pitch, angles.y); // Pitch
    // if (pitch_correction > -1 && pitch_correction < 1)
    // {
    //     pitch_correction = 0;
    // }
    float yaw_correction = updatePID(&yaw_pid, norm_yaw, angles.z); // Yaw

    // usartWriteString("  rc: ");
    // usartWriteNumber((int32_t)roll_correction);
    // usartWriteString("  pc: ");
    // usartWriteNumber((int32_t)pitch_correction);
    // usartWriteString("  yc: ");
    // usartWriteNumber((int32_t)yaw_correction);
    // usartWriteString("    \r");

    // Mix the values
    // float m1 = norm_throttle + PITCH_GAIN * pitch_correction - ROLL_GAIN * roll_correction - YAW_GAIN * yaw_correction;
    // float m2 = norm_throttle + PITCH_GAIN * pitch_correction + ROLL_GAIN * roll_correction + YAW_GAIN * yaw_correction;
    // float m3 = norm_throttle - PITCH_GAIN * pitch_correction - ROLL_GAIN * roll_correction + YAW_GAIN * yaw_correction;
    // float m4 = norm_throttle - PITCH_GAIN * pitch_correction + ROLL_GAIN * roll_correction - YAW_GAIN * yaw_correction;

    float m1 = norm_throttle + pitch_correction - roll_correction - yaw_correction;
    float m2 = norm_throttle + pitch_correction + roll_correction + yaw_correction;
    float m3 = norm_throttle - pitch_correction - roll_correction + yaw_correction;
    float m4 = norm_throttle - pitch_correction + roll_correction - yaw_correction;

    // Clamp mixed values to PWM range [Min Throttle - Max Throttle]
    duty_cycles->duty1 = (uint8_t)(clamp(m1, (float)MIN_THROTTLE, (float)MAX_THROTTLE));
    duty_cycles->duty2 = (uint8_t)(clamp(m2, (float)MIN_THROTTLE, (float)MAX_THROTTLE));
    duty_cycles->duty3 = (uint8_t)(clamp(m3, (float)MIN_THROTTLE, (float)MAX_THROTTLE));
    duty_cycles->duty4 = (uint8_t)(clamp(m4, (float)MIN_THROTTLE, (float)MAX_THROTTLE));

    // usartWriteString("  m1: ");
    // usartWriteNumber((int32_t)duty_cycles->duty1);
    // usartWriteString("  m2: ");
    // usartWriteNumber((int32_t)duty_cycles->duty2);
    // usartWriteString("  m3: ");
    // usartWriteNumber((int32_t)duty_cycles->duty3);
    // usartWriteString("  m4: ");
    // usartWriteNumber((int32_t)duty_cycles->duty4);
    // usartWriteString("    \r");

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
    // usartWriteString("Received Packets: ");
    // usartWriteNumber(packets_received);
    // usartWriteChar('\n');
    packets_received = 0;
    return;
}

/**
 * @brief Update the Madgwick Orientation estimation
 *
 * @param accel_data XYZ Accelerometer data
 * @param gyro_data XYZ Accelerometer data
 *
 * @return None
 *
 * @note Sets the global quaternion values
 */
void updateOrientation(int16_t *accel_data, int16_t *gyro_data)
{
    // Convert gyro values to rad/s
    float gx = gyro_data[0] * 0.0152671756f * RAD;
    float gy = gyro_data[1] * 0.0152671756f * RAD;
    float gz = gyro_data[2] * 0.0152671756f * RAD;

    MadgwickAHRSupdateIMU(gx, gy, gz, (float)accel_data[0], (float)accel_data[1], (float)accel_data[2]);

    Quaterntion temp_q = getQuaternion();
    quaternionToEuler(temp_q, &angles);
    angles.y *= -1;

    // usartWriteString("Roll ");
    // usartWriteNumber((int32_t)angles.x);
    // usartWriteString(" | ");
    // usartWriteString("Pitch ");
    // usartWriteNumber((int32_t)angles.y);
    // usartWriteString(" | ");
    // usartWriteString("Yaw ");
    // usartWriteNumber((int32_t)angles.z);
    // usartWriteString("   \r");

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
    // initModules();

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

    // IMU data structures
    int16_t accel_data[3] = {0, 0, 0};
    int16_t gyro_data[3] = {0, 0, 0};

    // State initializations
    enum RADIO_STATE radio_state = RECEIVE;
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

            // Get IMU data
            getAccelData(IMU2, accel_data);
            // getGyroData(IMU2, gyro_data);
            if (getIMUDataReady(IMU2))
            {
                setIMUDataReady(IMU2, 0);

                // getXLDataBuffer(accel_data);

                // getGyroDataBuffer(gyro_data);
                // usartWriteString("X ");
                // usartWriteNumber((int32_t)accel_data[0]);
                // usartWriteString(" | ");
                // usartWriteString("Y ");
                // usartWriteNumber((int32_t)accel_data[1]);
                // usartWriteString(" | ");
                // usartWriteString("Z ");
                // usartWriteNumber((int32_t)accel_data[2]);
                // usartWriteString("   \r");
            }

            usartWriteString("X ");
            usartWriteNumber((int32_t)accel_data[0]);
            usartWriteString(" | ");
            usartWriteString("Y ");
            usartWriteNumber((int32_t)accel_data[1]);
            usartWriteString(" | ");
            usartWriteString("Z ");
            usartWriteNumber((int32_t)accel_data[2]);
            usartWriteString("   \r");

            // Update quaternion estimation
            updateOrientation(accel_data, gyro_data);

            // Calculate motor duty cycles from packet
            calculateMotorDuty(packet, &duty_cycles);
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
                    onLED(GREEN_LED);
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
                setDuty(CH1, MIN_THROTTLE);
                setDuty(CH2, MIN_THROTTLE);
                setDuty(CH3, MIN_THROTTLE);
                setDuty(CH4, MIN_THROTTLE);

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
                    offLED(GREEN_LED);
                }
                else if (motor_state == OPERATING && packet.button == 5)
                {
                    // User has initiated a base power up
                    MIN_THROTTLE += 10;
                    MIN_THROTTLE = MIN_THROTTLE > MAX_THROTTLE ? MAX_THROTTLE : MIN_THROTTLE;
                }
                else if (motor_state == OPERATING && packet.button == 4)
                {
                    // User has initiated a power-down
                    MIN_THROTTLE -= 10;
                    MIN_THROTTLE = MIN_THROTTLE < 10 ? 10 : MIN_THROTTLE;
                }

                // usartWriteString("  m1: ");
                // usartWriteNumber((int32_t)duty_cycles.duty1);
                // usartWriteString("  m2: ");
                // usartWriteNumber((int32_t)duty_cycles.duty2);
                // usartWriteString("  m3: ");
                // usartWriteNumber((int32_t)duty_cycles.duty3);
                // usartWriteString("  m4: ");
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
