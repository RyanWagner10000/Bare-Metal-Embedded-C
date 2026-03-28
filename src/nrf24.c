/*
 * file: nrf24.c
 * description: file that contains the helper functions for the NRF24L01 radio module
 * author: Ryan Wagner
 * date: March 27, 2026
 * notes:
 */

#include "nrf24.h"

const uint8_t RX_ADDR_P0_BUFFER[ADDRESS_WIDTH] = {0x01, 0x02, 0x03, 0x04, 0x00};
const uint8_t RX_ADDR_P1_BUFFER[ADDRESS_WIDTH] = {0x06, 0x07, 0x08, 0x09, 0x0A};
const uint8_t RX_ADDR_P2_BUFFER[ADDRESS_WIDTH] = {0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
const uint8_t RX_ADDR_P3_BUFFER[ADDRESS_WIDTH] = {0x11, 0x12, 0x13, 0x14, 0x15};
const uint8_t RX_ADDR_P4_BUFFER[ADDRESS_WIDTH] = {0x16, 0x17, 0x18, 0x19, 0x1A};
const uint8_t RX_ADDR_P5_BUFFER[ADDRESS_WIDTH] = {0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

/**
 * @brief Initalize the registers on the radio module
 *
 * @param None
 *
 * @return None
 */
void initRadio(void)
{
    // Transfer buffers
    uint8_t buffer[MAX_BUFFER_SIZE];

    // Pull CSN low
    enableCSN_SPI3();

    // Disable and enable radio
    // disableCE_SPI3();
    // enableCE_SPI3();

    // Set Config register
    buffer[0] = W_REGISTER | CONFIG;
    buffer[1] = (uint32_t)0x00;
    transmitSPI3(buffer, 2);

    // Set Auto-Acknowledge register
    buffer[0] = W_REGISTER | EN_AA;
    buffer[1] = 0x3F;
    transmitSPI3(buffer, 2);

    // Set Rx Address register
    buffer[0] = W_REGISTER | EN_RXADDR;
    buffer[1] = 0x03;
    transmitSPI3(buffer, 2);

    // Set Address Widths register
    buffer[0] = W_REGISTER | SETUP_AW;
    buffer[1] = 0x03;
    transmitSPI3(buffer, 2);

    // Set Auto Retransmission regis
    buffer[0] = W_REGISTER | SETUP_RETR;
    buffer[1] = 0x00;
    transmitSPI3(buffer, 2);

    // Set RF Channel register
    buffer[0] = W_REGISTER | RF_CH;
    buffer[1] = 0;
    transmitSPI3(buffer, 2);

    // Set RF Setup register
    buffer[0] = W_REGISTER | RF_SETUP;
    buffer[1] = 0x0F;
    transmitSPI3(buffer, 2);

    // Set Rx Address Pipe 0 registe
    buffer[0] = W_REGISTER | RX_ADDR_P0;
    for (uint8_t i = 1; i < ADDRESS_WIDTH + 1; i++)
    {
        buffer[i] = RX_ADDR_P0_BUFFER[i - 1];
    }
    transmitSPI3(buffer, ADDRESS_WIDTH + 1);

    // Read Rx Address Pipe 0 register to confirm
    buffer[0] = RX_ADDR_P0;
    transmitSPI3(buffer, 1);
    for (uint8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        buffer[i] = NOP;
    }
    receiveSPI3(buffer, ADDRESS_WIDTH + 1);

    uint8_t incorrect = 0;
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        if (buffer[i] != RX_ADDR_P0_BUFFER[i])
        {
            incorrect = 1;
            usartWriteString("RX_ADDR_P0 read was incorrect.\n");
            usartWriteString("Received: ");
            usartWriteNumber((uint32_t)buffer[i]);
        }
    }
    if (incorrect)
    {
        usartWriteString("Reading the same data from RX_ADDR_P0 was unsuccessful.\n");
    }

    // Pull CSN high
    disableCSN_SPI3();

    // Flush Tx and Rx buffers with command
    flushRx();
    flushTx();

    return;
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

    // Config register
    value = readRegister(CONFIG);
    char config_str[MAX_INT_STRING];
    intToStr((int32_t)value, config_str);
    char config_msg[MAX_STRING_CONCAT];
    strConcat("CONFIG = ", config_str, config_msg);
    strConcat(config_msg, "\n", config_msg);
    usartWriteString(config_msg);

    // Enable Auto-acknowledge register
    value = readRegister(EN_AA);
    char enaa_str[MAX_INT_STRING];
    intToStr((int32_t)value, enaa_str);
    char enaa_msg[MAX_STRING_CONCAT];
    strConcat("EN_AA = ", enaa_str, enaa_msg);
    strConcat(enaa_msg, "\n", enaa_msg);
    usartWriteString(enaa_msg);

    // Enable Rx Address register
    value = readRegister(EN_RXADDR);
    char enrxaddr_str[MAX_INT_STRING];
    intToStr((int32_t)value, enrxaddr_str);
    char enrxaddr_msg[MAX_STRING_CONCAT];
    strConcat("EN_RXADDR = ", enrxaddr_str, enrxaddr_msg);
    strConcat(enrxaddr_msg, "\n", enrxaddr_msg);
    usartWriteString(enrxaddr_msg);

    // Setup Address Widths register
    value = readRegister(SETUP_AW);
    char setupaw_str[MAX_INT_STRING];
    intToStr((int32_t)value, setupaw_str);
    char setupaw_msg[MAX_STRING_CONCAT];
    strConcat("SETUP_AW = ", setupaw_str, setupaw_msg);
    strConcat(setupaw_msg, "\n", setupaw_msg);
    usartWriteString(setupaw_msg);

    // Setup Retransmission register
    value = readRegister(SETUP_RETR);
    char setupretr_str[MAX_INT_STRING];
    intToStr((int32_t)value, setupretr_str);
    char setupretr_msg[MAX_STRING_CONCAT];
    strConcat("SETUP_RETR = ", setupretr_str, setupretr_msg);
    strConcat(setupretr_msg, "\n", setupretr_msg);
    usartWriteString(setupretr_msg);

    // RF Channel register
    value = readRegister(RF_CH);
    char rfch_str[MAX_INT_STRING];
    intToStr((int32_t)value, rfch_str);
    char rfch_msg[MAX_STRING_CONCAT];
    strConcat("RF_CH = ", rfch_str, rfch_msg);
    strConcat(rfch_msg, "\n", rfch_msg);
    usartWriteString(rfch_msg);

    // RF Setup register
    value = readRegister(RF_SETUP);
    char rfsetup_str[MAX_INT_STRING];
    intToStr((int32_t)value, rfsetup_str);
    char rfsetup_msg[MAX_STRING_CONCAT];
    strConcat("RF_SETUP = ", rfsetup_str, rfsetup_msg);
    strConcat(rfsetup_msg, "\n", rfsetup_msg);
    usartWriteString(rfsetup_msg);

    // Status register
    value = readRegister(STATUS);
    char status_str[MAX_INT_STRING];
    intToStr((int32_t)value, status_str);
    char status_msg[MAX_STRING_CONCAT];
    strConcat("STATUS = ", status_str, status_msg);
    strConcat(status_msg, "\n", status_msg);
    usartWriteString(status_msg);

    // FIFO Status register
    value = readRegister(FIFO_STATUS);
    char fifostatus_str[MAX_INT_STRING];
    intToStr((int32_t)value, fifostatus_str);
    char fifostatus_msg[MAX_STRING_CONCAT];
    strConcat("FIFO_STATUS = ", fifostatus_str, fifostatus_msg);
    strConcat(fifostatus_msg, "\n", fifostatus_msg);
    usartWriteString(fifostatus_msg);
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
    uint8_t tx_buffer[1] = {0};
    uint8_t rx_buffer[1] = {0};
    uint8_t status = 0;

    enableCSN_SPI3();

    tx_buffer[0] = STATUS;
    transmitSPI3(tx_buffer, 1);
    receiveSPI3(rx_buffer, 1);
    status = rx_buffer[0];
    status_data.STATUS_BYTE = status;
    status_data.RX_DR = (status >> 6) & 1;
    status_data.TX_DS = (status >> 5) & 1;
    status_data.MAX_RT = (status >> 4) & 1;
    status_data.RX_P_NO = (status >> 1) & 7; // 3 bits
    status_data.TX_FULL = status & 1;

    status = 0;
    tx_buffer[0] = FIFO_STATUS;
    transmitSPI3(tx_buffer, 1);
    receiveSPI3(rx_buffer, 1);
    status = rx_buffer[0];
    status_data.FIFO_STATUS_BYTE = status;
    status_data.TX_REUSE = (status >> 6) & 1;
    status_data.TX_FULL_F = (status >> 5) & 1;
    status_data.TX_EMPTY = (status >> 4) & 1;
    status_data.RX_FULL = (status >> 1) & 1;
    status_data.RX_EMPTY = status & 1;

    status = 0;
    tx_buffer[0] = OBSERVE_TX;
    transmitSPI3(tx_buffer, 1);
    receiveSPI3(rx_buffer, 1);
    status = rx_buffer[0];
    status_data.PLOS_CNT = status & 15;
    status_data.ARC_CNT = (status >> 4) & 15;

    disableCSN_SPI3();

    return status_data;
}

/**
 * @brief Setup the NRF24L01 radio module in Tx mode
 *
 * @param channel Channel frequency to operate on
 *
 * @return None
 */
void setTxMode(uint8_t channel)
{
    // Disable radio
    enableCSN_SPI3();

    uint8_t tx_buffer[MAX_BUFFER_SIZE];
    uint8_t rx_buffer[1] = {0};

    // Set RF Channel
    tx_buffer[0] = W_REGISTER | RF_CH;
    tx_buffer[1] = channel;
    transmitSPI3(tx_buffer, 2);

    // Set Auto Acknowledge
    tx_buffer[0] = W_REGISTER | EN_AA;
    tx_buffer[1] = 0x00;
    transmitSPI3(tx_buffer, 2);

    // Set retransmission
    tx_buffer[0] = W_REGISTER | SETUP_RETR;
    tx_buffer[1] = 0x00;
    transmitSPI3(tx_buffer, 2);

    // Set Tx Address
    tx_buffer[0] = W_REGISTER | TX_ADDR;
    tx_buffer[1] = RX_ADDR_P0_BUFFER[0];
    tx_buffer[2] = RX_ADDR_P0_BUFFER[1];
    tx_buffer[3] = RX_ADDR_P0_BUFFER[2];
    tx_buffer[4] = RX_ADDR_P0_BUFFER[3];
    tx_buffer[5] = RX_ADDR_P0_BUFFER[4];
    transmitSPI3(tx_buffer, 6);

    // Get current config
    tx_buffer[0] = CONFIG;
    transmitSPI3(tx_buffer, 1);
    receiveSPI3(rx_buffer, 1);
    uint8_t config = rx_buffer[0];
    // Modify config register and send back
    config |= (1 << 1);
    config &= ~(1 << 0);
    tx_buffer[0] = W_REGISTER | CONFIG;
    tx_buffer[1] = config;
    transmitSPI3(tx_buffer, 2);

    // Enable radio again
    disableCSN_SPI3();
}

/**
 * @brief Setup the NRF24L01 radio module in Rx mode
 *
 * @param channel Channel frequency to operate on
 *
 * @return None
 */
void setRxMode(uint8_t channel)
{
    // Disable radio
    enableCSN_SPI3();

    uint8_t tx_buffer[MAX_BUFFER_SIZE];
    uint8_t rx_buffer[1] = {0};

    // Set RF Channel
    tx_buffer[0] = W_REGISTER | RF_CH;
    tx_buffer[1] = channel;
    transmitSPI3(tx_buffer, 2);

    // Set Payload size for Pipe 0
    tx_buffer[0] = W_REGISTER | RX_PW_P0;
    tx_buffer[1] = P0_PACKET_SIZE;
    transmitSPI3(tx_buffer, 2);

    // Set Tx Address
    tx_buffer[0] = W_REGISTER | RX_ADDR_P0;
    tx_buffer[1] = RX_ADDR_P0_BUFFER[0];
    tx_buffer[2] = RX_ADDR_P0_BUFFER[1];
    tx_buffer[3] = RX_ADDR_P0_BUFFER[2];
    tx_buffer[4] = RX_ADDR_P0_BUFFER[3];
    tx_buffer[5] = RX_ADDR_P0_BUFFER[4];
    transmitSPI3(tx_buffer, 6);

    // Get current config
    tx_buffer[0] = CONFIG;
    transmitSPI3(tx_buffer, 1);
    receiveSPI3(rx_buffer, 1);
    uint8_t config = rx_buffer[0];
    // Modify config register and send back
    config = config | (1 << 1) | (1 << 1);
    tx_buffer[0] = W_REGISTER | CONFIG;
    tx_buffer[1] = config;
    transmitSPI3(tx_buffer, 2);

    // Enable radio again
    disableCSN_SPI3();
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
    // Disable to modify Tx FIFO
    enableCSN_SPI3();

    // Flush buffers
    flushRx();
    flushTx();

    // Clamp the size of the data to within limit
    length = length < P0_PACKET_SIZE ? length : P0_PACKET_SIZE;

    // Load data into txbuffer
    uint8_t tx_buffer[MAX_BUFFER_SIZE];
    tx_buffer[0] = W_TX_PAYLOAD;
    for (uint8_t i = 1; i < length + 1; i++)
    {
        tx_buffer[i] = data[i - 1];
    }
    transmitSPI3(tx_buffer, length + 1);

    // Toggle radio for at least 10us to transmit data
    enableCE_SPI3();
    delayMicrosecond(15);
    disableCE_SPI3();

    // Enable to transmit
    disableCSN_SPI3();
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

    if (status.RX_P_NO < 7)
    {
        return 1;
    }

    return 0;
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
void readRadio(uint8_t *data, enum PIPE_PACKET_SIZE pps)
{
    uint8_t buffer[1] = {R_RX_PAYLOAD};

    enableCE_SPI3();
    enableCSN_SPI3();

    // Read Rx FIFO register
    transmitSPI3(buffer, 1);
    receiveSPI3(data, pps);

    delayMicrosecond(100);

    disableCSN_SPI3();
    disableCE_SPI3();

    flushRx();

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
    uint8_t buffer[1] = {FLUSH_RX};
    enableCSN_SPI3();
    transmitSPI3(buffer, 1);
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
    uint8_t buffer[1] = {FLUSH_TX};
    enableCSN_SPI3();
    transmitSPI3(buffer, 1);
    disableCSN_SPI3();
    delayMicrosecond(130);
    return;
}

/**
 * @brief Read the bits of a specific register on the radio module
 *
 * @param address Register address on radio module
 *
 * @return None
 */
uint8_t readRegister(uint8_t address)
{
    // Format message
    uint8_t tx_buffer[1] = {address};
    uint8_t rx_buffer[1] = {0};

    // Pull low
    enableCSN_SPI3();

    // Send read operation
    transmitSPI3(tx_buffer, 1);

    // Get data back
    receiveSPI3(rx_buffer, 1);

    // Pull high
    disableCSN_SPI3();

    return rx_buffer[0];
}