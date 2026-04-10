/*
 * file: nrf24.c
 * description: file that contains the helper functions for the NRF24L01 radio module
 * author: Ryan Wagner
 * date: March 27, 2026
 * notes:
 */

#include "nrf24.h"

uint8_t RX_ADDR_P0_BUFFER[ADDRESS_WIDTH] = {0x01, 0x02, 0x03, 0x04, 0x00};
uint8_t RX_ADDR_P1_BUFFER[ADDRESS_WIDTH] = {0x06, 0x07, 0x08, 0x09, 0x0A};
uint8_t RX_ADDR_P2_BUFFER[ADDRESS_WIDTH] = {0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
uint8_t RX_ADDR_P3_BUFFER[ADDRESS_WIDTH] = {0x11, 0x12, 0x13, 0x14, 0x15};
uint8_t RX_ADDR_P4_BUFFER[ADDRESS_WIDTH] = {0x16, 0x17, 0x18, 0x19, 0x1A};
uint8_t RX_ADDR_P5_BUFFER[ADDRESS_WIDTH] = {0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

/**
 * @brief Internal function to write byte to specific register on the radio module
 *
 * @param address Register address on radio module
 * @param value Value to set register on the module
 *
 * @return None
 */
void writeRegisterSingle(uint8_t address, uint8_t value)
{
    uint8_t tx_buffer[2] = {(W_REGISTER | address), value};
    uint8_t rx_buffer[2] = {0, 0};

    // Set CSN pin LOW
    enableCSN_SPI3();

    // Send write operation
    transferSPI3(tx_buffer, rx_buffer, 2);

    // Set CSN pin HIGH
    disableCSN_SPI3();

    return;
}

/**
 * @brief Internal function to write N-many bytes to specific register on the radio module
 *
 * @param address Register address on radio module
 * @param value Value array to set register on the module
 * @param length Length of value array
 *
 * @return None
 */
void writeRegisterMulti(uint8_t address, uint8_t *value, uint8_t length)
{
    uint8_t tx_buffer[MAX_BUFFER_SIZE];
    uint8_t rx_buffer[MAX_BUFFER_SIZE];

    length = length <= MAX_BUFFER_SIZE ? length : MAX_BUFFER_SIZE;

    // Copy data into Tx buffer
    tx_buffer[0] = W_REGISTER | address;
    for (uint8_t i = 1; i < length + 1; i++)
    {
        tx_buffer[i] = value[i - 1];
    }

    // Set CSN pin LOW
    enableCSN_SPI3();

    // Send write operation
    transferSPI3(tx_buffer, rx_buffer, length + 1);

    // Set CSN pin HIGH
    disableCSN_SPI3();

    return;
}

/**
 * @brief Internal function to read byte of specific register on the radio module
 *
 * @param address Register address on radio module
 *
 * @return None
 */
uint8_t readRegisterSingle(uint8_t address)
{
    // Format message
    uint8_t tx_buffer[2] = {address, 0x00};
    uint8_t rx_buffer[2] = {0, 0};

    // Set CSN pin LOW
    enableCSN_SPI3();

    // Send read operation
    transferSPI3(tx_buffer, rx_buffer, 2);

    // Set CSN pin HIGH
    disableCSN_SPI3();

    return rx_buffer[1];
}

/**
 * @brief Internal function to read N-many bytes from specific register on the radio module
 *
 * @param address Register address on radio module
 * @param values Values array to get from registers on the module
 * @param length Length of value array
 *
 * @return None
 */
void readRegisterMulti(uint8_t address, uint8_t *values, uint8_t length)
{
    uint8_t tx_buffer[MAX_BUFFER_SIZE];
    uint8_t rx_buffer[MAX_BUFFER_SIZE];

    length = length <= MAX_BUFFER_SIZE ? length : MAX_BUFFER_SIZE;

    // Copy data into Tx buffer
    tx_buffer[0] = address;
    for (uint8_t i = 1; i < length + 1; i++)
    {
        tx_buffer[i] = 0x00;
    }

    // Set CSN pin LOW
    enableCSN_SPI3();

    // Send write operation
    transferSPI3(tx_buffer, rx_buffer, length + 1);

    // Set CSN pin HIGH
    disableCSN_SPI3();

    // Transfer data into values array
    for (int8_t i = 0; i < length; i++)
    {
        values[i] = rx_buffer[i + 1];
    }

    return;
}

/**
 * @brief Initalize the registers on the radio module
 *
 * @param channel RF Channel frequency offset
 *
 * @return 0 on failure, 1 on success
 */
uint8_t initRadio(uint8_t channel)
{
    uint8_t success = 1;
    uint8_t check_value = 0xFF;

    // Settings
    uint8_t config = 0x00;
    // uint8_t en_aa = 0x3F;
    uint8_t en_aa = 0x00;
    uint8_t en_rxaddr = 0x03;
    uint8_t setup_aw = 0x03;
    uint8_t setup_retr = 0x00;
    uint8_t rf_ch = channel;
    uint8_t rf_setup = 0x0F;

    // Disable radio TxRx
    disableCE_SPI3();

    // Set Config register
    writeRegisterSingle(CONFIG, config);
    // Check value
    check_value = readRegisterSingle(CONFIG);
    if (check_value != config)
        success = 0;
    check_value = 0xFF;

    // Set Auto-Acknowledge register
    writeRegisterSingle(EN_AA, en_aa);
    // Check value
    check_value = readRegisterSingle(EN_AA);
    if (check_value != en_aa)
        success = 0;
    check_value = 0xFF;

    // Set Rx Address register
    writeRegisterSingle(EN_RXADDR, en_rxaddr);
    // Check value
    check_value = readRegisterSingle(EN_RXADDR);
    if (check_value != en_rxaddr)
        success = 0;
    check_value = 0xFF;

    // Set Address Widths register
    writeRegisterSingle(SETUP_AW, setup_aw);
    // Check value
    check_value = readRegisterSingle(SETUP_AW);
    if (check_value != setup_aw)
        success = 0;
    check_value = 0xFF;

    // Set Auto Retransmission regis
    writeRegisterSingle(SETUP_RETR, setup_retr);
    // Check value
    check_value = readRegisterSingle(SETUP_RETR);
    if (check_value != setup_retr)
        success = 0;
    check_value = 0xFF;

    // Set RF Channel register
    writeRegisterSingle(RF_CH, rf_ch);
    // Check value
    check_value = readRegisterSingle(RF_CH);
    if (check_value != rf_ch)
        success = 0;
    check_value = 0xFF;

    // Set RF Setup register
    writeRegisterSingle(RF_SETUP, rf_setup);
    // Check value
    check_value = readRegisterSingle(RF_SETUP);
    if (check_value != rf_setup)
        success = 0;
    check_value = 0xFF;

    // Set Payload size for Pipe 0
    writeRegisterSingle(RX_PW_P0, P0_PACKET_SIZE);
    // Check value
    check_value = readRegisterSingle(RX_PW_P0);
    if (check_value != P0_PACKET_SIZE)
        success = 0;
    check_value = 0xFF;

    // Set Rx Address Pipe 0 register
    writeRegisterMulti(RX_ADDR_P0, RX_ADDR_P0_BUFFER, ADDRESS_WIDTH);

    // Read Rx Address Pipe 0 register to confirm
    uint8_t read_buffer[ADDRESS_WIDTH];
    readRegisterMulti(RX_ADDR_P0, read_buffer, ADDRESS_WIDTH);

    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        if (read_buffer[i] != RX_ADDR_P0_BUFFER[i])
        {
            usartWriteString("Reading the same data from RX_ADDR_P0 was unsuccessful.\n");
            break;
        }
        // Reset for next use
        read_buffer[i] = 0;
    }

    // Set Tx Address
    writeRegisterMulti(TX_ADDR, RX_ADDR_P0_BUFFER, ADDRESS_WIDTH);
    // Check values
    readRegisterMulti(TX_ADDR, read_buffer, ADDRESS_WIDTH);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        if (read_buffer[i] != RX_ADDR_P0_BUFFER[i])
        {
            success = 0;
            break;
        }
        // Reset for next use
        read_buffer[i] = 0;
    }

    // Flush Tx and Rx buffers with command
    flushRx();
    flushTx();

    return success;
}

/**
 * @brief Print all the radio registers
 *
 * @param None
 *
 * @return None
 */
void printRadioSettings(void)
{
    uint8_t value = 0;

    usartWriteString("\n -- Current NRF24L01 Radio Setting -- \n");

    // Config register
    value = readRegisterSingle(CONFIG);
    char config_str[MAX_INT_STRING];
    intToStr((int32_t)value, config_str);
    char config_msg[MAX_STRING_CONCAT];
    strConcat("CONFIG = ", config_str, config_msg);
    strConcat(config_msg, "\n", config_msg);
    usartWriteString(config_msg);

    // Enable Auto-acknowledge register
    value = readRegisterSingle(EN_AA);
    char enaa_str[MAX_INT_STRING];
    intToStr((int32_t)value, enaa_str);
    char enaa_msg[MAX_STRING_CONCAT];
    strConcat("EN_AA = ", enaa_str, enaa_msg);
    strConcat(enaa_msg, "\n", enaa_msg);
    usartWriteString(enaa_msg);

    // Enable Rx Address register
    value = readRegisterSingle(EN_RXADDR);
    char enrxaddr_str[MAX_INT_STRING];
    intToStr((int32_t)value, enrxaddr_str);
    char enrxaddr_msg[MAX_STRING_CONCAT];
    strConcat("EN_RXADDR = ", enrxaddr_str, enrxaddr_msg);
    strConcat(enrxaddr_msg, "\n", enrxaddr_msg);
    usartWriteString(enrxaddr_msg);

    // Setup Address Widths register
    value = readRegisterSingle(SETUP_AW);
    char setupaw_str[MAX_INT_STRING];
    intToStr((int32_t)value, setupaw_str);
    char setupaw_msg[MAX_STRING_CONCAT];
    strConcat("SETUP_AW = ", setupaw_str, setupaw_msg);
    strConcat(setupaw_msg, "\n", setupaw_msg);
    usartWriteString(setupaw_msg);

    // Setup Retransmission register
    value = readRegisterSingle(SETUP_RETR);
    char setupretr_str[MAX_INT_STRING];
    intToStr((int32_t)value, setupretr_str);
    char setupretr_msg[MAX_STRING_CONCAT];
    strConcat("SETUP_RETR = ", setupretr_str, setupretr_msg);
    strConcat(setupretr_msg, "\n", setupretr_msg);
    usartWriteString(setupretr_msg);

    // RF Channel register
    value = readRegisterSingle(RF_CH);
    char rfch_str[MAX_INT_STRING];
    intToStr((int32_t)value, rfch_str);
    char rfch_msg[MAX_STRING_CONCAT];
    strConcat("RF_CH = ", rfch_str, rfch_msg);
    strConcat(rfch_msg, "\n", rfch_msg);
    usartWriteString(rfch_msg);

    // RF Setup register
    value = readRegisterSingle(RF_SETUP);
    char rfsetup_str[MAX_INT_STRING];
    intToStr((int32_t)value, rfsetup_str);
    char rfsetup_msg[MAX_STRING_CONCAT];
    strConcat("RF_SETUP = ", rfsetup_str, rfsetup_msg);
    strConcat(rfsetup_msg, "\n", rfsetup_msg);
    usartWriteString(rfsetup_msg);

    // Status register
    value = readRegisterSingle(STATUS);
    char status_str[MAX_INT_STRING];
    intToStr((int32_t)value, status_str);
    char status_msg[MAX_STRING_CONCAT];
    strConcat("STATUS = ", status_str, status_msg);
    strConcat(status_msg, "\n", status_msg);
    usartWriteString(status_msg);

    // FIFO Status register
    value = readRegisterSingle(FIFO_STATUS);
    char fifostatus_str[MAX_INT_STRING];
    intToStr((int32_t)value, fifostatus_str);
    char fifostatus_msg[MAX_STRING_CONCAT];
    strConcat("FIFO_STATUS = ", fifostatus_str, fifostatus_msg);
    strConcat(fifostatus_msg, "\n", fifostatus_msg);
    usartWriteString(fifostatus_msg);

    // TX_ADDR register
    uint8_t read_buffer[ADDRESS_WIDTH];
    readRegisterMulti(TX_ADDR, read_buffer, ADDRESS_WIDTH);
    char tx_addr_msg[MAX_STRING_CONCAT];
    strConcat("TX_ADDR = {", "", tx_addr_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char tx_addr_str[MAX_INT_STRING];
        intToStr((int32_t)read_buffer[i], tx_addr_str);
        // Concat
        strConcat(tx_addr_msg, tx_addr_str, tx_addr_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            strConcat(tx_addr_msg, ", ", tx_addr_msg);
    }
    strConcat(tx_addr_msg, "}\n", tx_addr_msg);
    usartWriteString(tx_addr_msg);

    // RX_ADDR_P0 register
    readRegisterMulti(RX_ADDR_P0, read_buffer, ADDRESS_WIDTH);
    char rx_addr0_msg[MAX_STRING_CONCAT];
    strConcat("RX_ADDR_P0 = {", "", rx_addr0_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr0_str[MAX_INT_STRING];
        intToStr((int32_t)read_buffer[i], rx_addr0_str);
        // Concat
        strConcat(rx_addr0_msg, rx_addr0_str, rx_addr0_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            strConcat(rx_addr0_msg, ", ", rx_addr0_msg);
    }
    strConcat(rx_addr0_msg, "}\n", rx_addr0_msg);
    usartWriteString(rx_addr0_msg);

    // RX_ADDR_P1 register
    readRegisterMulti(RX_ADDR_P1, read_buffer, ADDRESS_WIDTH);
    char rx_addr1_msg[MAX_STRING_CONCAT];
    strConcat("RX_ADDR_P1 = {", "", rx_addr1_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr1_str[MAX_INT_STRING];
        intToStr((int32_t)read_buffer[i], rx_addr1_str);
        // Concat
        strConcat(rx_addr1_msg, rx_addr1_str, rx_addr1_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            strConcat(rx_addr1_msg, ", ", rx_addr1_msg);
    }
    strConcat(rx_addr1_msg, "}\n", rx_addr1_msg);
    usartWriteString(rx_addr1_msg);

    // RX_ADDR_P2 register
    readRegisterMulti(RX_ADDR_P2, read_buffer, ADDRESS_WIDTH);
    char rx_addr2_msg[MAX_STRING_CONCAT];
    strConcat("RX_ADDR_P2 = {", "", rx_addr2_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr2_str[MAX_INT_STRING];
        intToStr((int32_t)read_buffer[i], rx_addr2_str);
        // Concat
        strConcat(rx_addr2_msg, rx_addr2_str, rx_addr2_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            strConcat(rx_addr2_msg, ", ", rx_addr2_msg);
    }
    strConcat(rx_addr2_msg, "}\n", rx_addr2_msg);
    usartWriteString(rx_addr2_msg);

    // RX_ADDR_P3 register
    readRegisterMulti(RX_ADDR_P3, read_buffer, ADDRESS_WIDTH);
    char rx_addr3_msg[MAX_STRING_CONCAT];
    strConcat("RX_ADDR_P3 = {", "", rx_addr3_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr3_str[MAX_INT_STRING];
        intToStr((int32_t)read_buffer[i], rx_addr3_str);
        // Concat
        strConcat(rx_addr3_msg, rx_addr3_str, rx_addr3_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            strConcat(rx_addr3_msg, ", ", rx_addr3_msg);
    }
    strConcat(rx_addr3_msg, "}\n", rx_addr3_msg);
    usartWriteString(rx_addr3_msg);

    // RX_ADDR_P4 register
    readRegisterMulti(RX_ADDR_P4, read_buffer, ADDRESS_WIDTH);
    char rx_addr4_msg[MAX_STRING_CONCAT];
    strConcat("RX_ADDR_P4 = {", "", rx_addr4_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr4_str[MAX_INT_STRING];
        intToStr((int32_t)read_buffer[i], rx_addr4_str);
        // Concat
        strConcat(rx_addr4_msg, rx_addr4_str, rx_addr4_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            strConcat(rx_addr4_msg, ", ", rx_addr4_msg);
    }
    strConcat(rx_addr4_msg, "}\n", rx_addr4_msg);
    usartWriteString(rx_addr4_msg);

    // RX_ADDR_P5 register
    readRegisterMulti(RX_ADDR_P5, read_buffer, ADDRESS_WIDTH);
    char rx_addr5_msg[MAX_STRING_CONCAT];
    strConcat("RX_ADDR_P5 = {", "", rx_addr5_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr5_str[MAX_INT_STRING];
        intToStr((int32_t)read_buffer[i], rx_addr5_str);
        // Concat
        strConcat(rx_addr5_msg, rx_addr5_str, rx_addr5_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            strConcat(rx_addr5_msg, ", ", rx_addr5_msg);
    }
    strConcat(rx_addr5_msg, "}\n", rx_addr5_msg);
    usartWriteString(rx_addr5_msg);

    usartWriteString("\n\n");

    return;
}

/**
 * @brief
 *
 * @param None
 *
 * @return None
 */
struct NRF24_STATUS_DATA statusRadio(void)
{
    struct NRF24_STATUS_DATA status_data;
    uint8_t status = 0;

    status = readRegisterSingle(STATUS);
    status_data.STATUS_BYTE = status;
    status_data.RX_DR = (status >> 6) & 1;
    status_data.TX_DS = (status >> 5) & 1;
    status_data.MAX_RT = (status >> 4) & 1;
    status_data.RX_P_NO = (status >> 1) & 7; // 3 bits
    status_data.TX_FULL = status & 1;

    // status = 0;
    // status = readRegisterSingle(FIFO_STATUS);
    // status_data.FIFO_STATUS_BYTE = status;
    // status_data.TX_REUSE = (status >> 6) & 1;
    // status_data.TX_FULL_F = (status >> 5) & 1;
    // status_data.TX_EMPTY = (status >> 4) & 1;
    // status_data.RX_FULL = (status >> 1) & 1;
    // status_data.RX_EMPTY = status & 1;

    // status = 0;
    // status = readRegisterSingle(OBSERVE_TX);
    // status_data.PLOS_CNT = status & 15;
    // status_data.ARC_CNT = (status >> 4) & 15;

    return status_data;
}

/**
 * @brief Setup the NRF24L01 radio module in Tx mode
 *
 * @param None
 *
 * @return 0 on failure, 1 on success
 */
uint8_t setTxMode(void)
{
    uint8_t success = 1;
    uint8_t check_value = 0xFF;

    // Turn Radio OFF
    disableCE_SPI3();

    // Get current config
    uint8_t config = readRegisterSingle(CONFIG);
    // Modify config register and send back
    config |= (1 << 1);
    config &= ~(1 << 0);
    writeRegisterSingle(CONFIG, config);
    // Check value
    check_value = readRegisterSingle(CONFIG);
    if (check_value != config)
        success = 0;

    // Turn Radio ON
    enableCE_SPI3();

    return success;
}

/**
 * @brief Setup the NRF24L01 radio module in Rx mode
 *
 * @param None
 *
 * @return 0 on failure, 1 on success
 */
uint8_t setRxMode(void)
{
    uint8_t success = 1;
    uint8_t check_value = 0xFF;

    // Turn Radio OFF
    disableCE_SPI3();

    // Get current config
    uint8_t config = readRegisterSingle(CONFIG);
    // Modify config register and send back
    config |= (1 << 1);
    config |= (1 << 0);
    writeRegisterSingle(CONFIG, config);
    // Check value
    check_value = readRegisterSingle(CONFIG);
    if (check_value != config)
        success = 0;

    // Turn Radio ON
    enableCE_SPI3();

    return success;
}

/**
 * @brief Transmit data from the radio
 *
 * @param data Array of values to transmit
 * @param length Length of data array
 *
 * @return None
 */
void transmitRadio(uint8_t *data, uint8_t length)
{
    // Flush buffers
    // flushRx();
    flushTx();

    // Clamp the size of the data to within limit
    length = length < P0_PACKET_SIZE ? length : P0_PACKET_SIZE;

    // Make sure radio is OFF
    disableCE_SPI3();

    // Load data into txbuffer
    writeRegisterMulti(W_TX_PAYLOAD, data, length);

    // Toggle radio for at least 10us to transmit data
    enableCE_SPI3();
    delayMicrosecond(15);
    disableCE_SPI3();

    return;
}

/**
 * @brief Check to see is data is available to read on the FIFO
 *
 * @param None
 *
 * @return 0 on none available, 1 on available
 */
uint8_t dataAvailable(void)
{
    struct NRF24_STATUS_DATA status;

    status = statusRadio();

    return status.RX_P_NO < 7 ? 1 : 0;
}

/**
 * @brief Check to see if the Tx FIFO is full
 *
 * @param None
 *
 * @return 0 on not full, 1 on full
 */
uint8_t txFIFOFull(void)
{
    struct NRF24_STATUS_DATA status;

    status = statusRadio();

    return status.TX_FULL;
}

/**
 * @brief Read the radio Rx FIFO
 *
 * @param data Array of values to transmit
 * @param pps Pipe number correlatred to packet size
 *
 * @return None
 */
void readRadio(RadioPacket *packet, enum PIPE_PACKET_SIZE pps)
{
    // Make sure the array is aligned in memory
    // So that when it gets cast to the struct it lines up
    uint8_t __attribute__((aligned(2))) rx_buffer[pps];

    // Read Rx FIFO register
    readRegisterMulti(R_RX_PAYLOAD, rx_buffer, pps);

    // Delay per documentation
    delayMicrosecond(100);

    // Turn radio on
    enableCE_SPI3();

    flushRx();

    // Cast data array into struct
    *packet = *(RadioPacket *)rx_buffer;

    return;
}

/**
 * @brief Use the built-in command to flush the Rx buffer on the radio module
 *
 * @param None
 *
 * @return None
 */
void flushRx(void)
{
    uint8_t tx_buffer[2] = {FLUSH_RX, 0};
    uint8_t rx_buffer[2] = {0, 0};

    // Set CSN pin LOW
    enableCSN_SPI3();

    // Send write operation
    transferSPI3(tx_buffer, rx_buffer, 2);

    // Set CSN pin HIGH
    disableCSN_SPI3();

    delayMicrosecond(130);
    return;
}

/**
 * @brief Use the built-in command to flush the Tx buffer on the radio module
 *
 * @param None
 *
 * @return None
 */
void flushTx(void)
{
    uint8_t tx_buffer[2] = {FLUSH_TX, 0};
    uint8_t rx_buffer[2] = {0, 0};

    // Set CSN pin LOW
    enableCSN_SPI3();

    // Send write operation
    transferSPI3(tx_buffer, rx_buffer, 2);

    // Set CSN pin HIGH
    disableCSN_SPI3();

    delayMicrosecond(130);
    return;
}

/**
 * @brief Print the contents of a packet
 *
 * @param packet RadioPacket object to print
 *
 * @return None
 */
void printPacket(RadioPacket packet)
{
    usartWriteString("packet_id = ");
    usartWriteNumber((int32_t)packet.packet_id);
    usartWriteChar('\n');
    usartWriteString("flags = ");
    usartWriteNumber((int32_t)packet.flags);
    usartWriteChar('\n');
    usartWriteString("throttle = ");
    usartWriteNumber((int32_t)packet.throttle);
    usartWriteChar('\n');
    usartWriteString("roll = ");
    usartWriteNumber((int32_t)packet.roll);
    usartWriteChar('\n');
    usartWriteString("pitch = ");
    usartWriteNumber((int32_t)packet.pitch);
    usartWriteChar('\n');
    usartWriteString("yaw = ");
    usartWriteNumber((int32_t)packet.yaw);
    usartWriteChar('\n');
    usartWriteString("button = ");
    usartWriteNumber((int32_t)packet.button);
    usartWriteChar('\n');
    usartWriteString("checksum = ");
    usartWriteNumber((int32_t)packet.checksum);
    usartWriteChar('\n');
    usartWriteChar('\n');

    return;
}