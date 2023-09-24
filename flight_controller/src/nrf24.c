#include <stdbool.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "spi1.h"
#include "nrf24.h"

#define PAYLOAD_LENGTH 32U

static void write_register(uint8_t reg, uint8_t value)
{
    cs_enable();
    spi1_send_byte(0x20 | (0x1F & reg));
    spi1_send_byte(value);
    cs_disable();
}

static uint8_t read_register(uint8_t reg)
{
    cs_enable();
    spi1_send_byte(0x1F & reg);
    uint8_t data = spi1_send_byte(reg);
    cs_disable();

    return data;
}

static void flush_rx(void)
{
    cs_enable();
    spi1_send_byte(0xE2);
    cs_disable();
}

static void flush_tx(void)
{
    cs_enable();
    spi1_send_byte(0xE1);
    cs_disable();
}

bool trx_init(uint8_t address[5], uint8_t sizeof_address, uint8_t channel)
{
    uint8_t dump[5];

    // Initialize SPI1
    spi1_master_init();

    // Initialize PC13 for CE line
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH |= GPIO_CRH_MODE13;                      // 0b11 output max 50MHz
    GPIOC->CRH &= ~(GPIO_CRH_CNF13);                    // 0b00 output push-pull

    // Set CE low to put chip into the standby mode
    GPIOC->ODR &= ~GPIO_ODR_ODR13;
    for(int i = 0; i < 100000; i++);

    // Enter power down mode PWR_UP = 0 for configuring registers
    write_register(0x00, 0x00);

    // Enable auto acknowledgment for data pipe 0
    write_register(0x01, 1 << 0);

    // Enable data pipe 0
    write_register(0x02, 1 << 0);

    // Set TX/RX address width to 5 bytes, AW = 11
    write_register(0x03, (1 << 1 | 1 << 0));

    // Set auto retransmit delay ARD = 0101 = 1500us & set auto retransmit count ARC = 15
    write_register(0x04, read_register(0x04) | (1 << 6 | 1 << 4 | 0x0F));

    // Set channel
    write_register(0x05, channel);

    // Set data rate to 1Mbps & PA to 0dbm
    write_register(0x06, (1 << 1 | 1 << 2));

    // Set data pipe 0 width = 32bytes RX_PW_P0
    write_register(0x11, PAYLOAD_LENGTH);

    // Set TX address TX_ADDR
    cs_enable();
    spi1_send_byte(0x20 | 0x10);
    spi1_buffer_transaction(address, dump, sizeof_address);
    cs_disable();

    // Set RX pipe 0 address RX_ADDR_P0
    cs_enable();
    spi1_send_byte(0x20 | 0x0A);
    spi1_buffer_transaction(address, dump, sizeof_address);
    cs_disable();

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));

    flush_rx();
    flush_tx();

    // Set CRC & CRCO coding scheme to 2 bytes & power up the chip PWR_UP = 1
    write_register(0x00, (1 << 3 | 1 << 2 | 1 << 1));
    for(int i = 0; i < 100000; i++);

    return (1 << 3 | 1 << 2 | 1 << 1) == read_register(0x00);
}

bool trx_switch_tx(void)
{
    // Set CE low to put chip into the standby mode
    GPIOC->ODR &= ~GPIO_ODR_ODR13;

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));

    flush_tx();
    flush_rx();

    // Disable receiver PRIM_RX = 0
    write_register(0x00, read_register(0x00) & ~(1 << 0));
    for(int i = 0; i < 100000; i++);

    return !(read_register(0x00) & (1 << 0));
}

void trx_transmit(uint8_t *payload, uint8_t sizeof_payload)
{
    uint8_t dump[PAYLOAD_LENGTH];
    uint8_t tx_fifo[PAYLOAD_LENGTH];

    uint8_t num_of_bytes = (sizeof_payload / sizeof(uint8_t));

    for (uint8_t i = 0; i < PAYLOAD_LENGTH; i++) {

        if (i < num_of_bytes) {
            tx_fifo[i] = *(payload + i);
        } else {
            tx_fifo[i] = 0x00;
        }
    }

    // Set CE low to put chip into the standby mode
    GPIOC->ODR &= ~GPIO_ODR_ODR13;

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));

    cs_enable();
    spi1_send_byte(0xA0);
    spi1_buffer_transaction(tx_fifo, dump, sizeof tx_fifo);
    cs_disable();

    // Set CE high to start transmitting
    GPIOC->ODR |= GPIO_ODR_ODR13;

    // Check any one the bit to be set TX_DS & MAX_RT
    while (!(read_register(0x07) & (1 << 5 | 1 << 4)));

    // Set CE low to put chip into the standby mode
    GPIOC->ODR &= ~GPIO_ODR_ODR13;
}

bool trx_switch_rx(void)
{
    // Set CE low to put chip into the standby mode
    GPIOC->ODR &= ~GPIO_ODR_ODR13;

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));

    flush_rx();
    flush_tx();

    // Enable receiver PRIM_RX = 1
    write_register(0x00, read_register(0x00) | (1 << 0));

    // Set CE high to start listening
    GPIOC->ODR |= GPIO_ODR_ODR13;
    for(int i = 0; i < 1000000; i++);

    return read_register(0x00) & (1 << 0);
}

void trx_receive(uint8_t *buffer, uint8_t sizeof_buffer)
{
    cs_enable();
    spi1_send_byte(0x61);
    spi1_buffer_transaction(buffer, buffer, sizeof_buffer);
    cs_disable();

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));
}

bool trx_data_available(void)
{
    // Check if RX_EMPTY flag is not set
    return !(read_register(0x17) & (1 << 0));
}

void trx_dump_memory(char out[])
{   
    const uint8_t registers[] = {
        0x00,
        0x01,
        0x02,
        0x03,
        0x04,
        0x05,
        0x06,
        0x07,
        0x08,
        0x09,
        0x11,
        0x12,
        0x13,
        0x14,
        0x15,
        0x16,
        0x17,
        0x1C,
        0x1D
    };
    uint8_t data[19];

    for (uint8_t i = 0; i < (sizeof(registers) / sizeof(uint8_t)); i++) {
        *(data + i) = read_register(registers[i]);
    }
    sprintf(out, "00: 0x%X\n01: 0x%X\n02: 0x%X\n03: 0x%X\n04: 0x%X\n05: 0x%X\n06: 0x%X\n07: 0x%X\n08: 0x%X\n09: 0x%X\n11: 0x%X\n12: 0x%X\n13: 0x%X\n14: 0x%X\n15: 0x%X\n16: 0x%X\n17: 0x%X\n1C: 0x%X\n1D: 0x%X\n\n\n",
        data[0],
        data[1],
        data[2],
        data[3],
        data[4],
        data[5],
        data[6],
        data[7],
        data[8],
        data[9],
        data[10],
        data[11],
        data[12],
        data[13],
        data[14],
        data[15],
        data[16],
        data[17],
        data[18]
    );
}
