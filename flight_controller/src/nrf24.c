#include <stdbool.h>
#include "stm32f1xx.h"
#include "spi1.h"

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
    spi1_send_byte(0xE2);
    cs_disable();
}

bool init_trx(void)
{
    // Set CE low to put chip into the standby mode
    GPIOC->ODR &= ~GPIO_ODR_ODR13;

    for(int i = 0; i < 100000; i++);

    // Set CRC & CRCO coding scheme to 2 bytes
    write_register(0x00, (1 << 2 | 1 << 3));

    // Enable auto acknowledgment for data pipe 0
    write_register(0x01, 1 << 0);

    // Enable data pipe 0
    write_register(0x02, 1 << 0);

    // Set TX/RX address width to 5 bytes, AW = 11
    write_register(0x03, (1 << 1 | 1 << 0));

    // Set auto retransmit delay ARD = 0101 = 1500us & set auto retransmit count ARC = 15
    write_register(0x04, read_register(0x04) | (1 << 6 | 1 << 4 | 0x0F));

    // Set channel to 76 = 2476MHz
    write_register(0x05, 76U);

    // Set data rate to 1Mbps & PA to 0dbm
    write_register(0x06, (1 << 1 | 1 << 2));

    // Set data pipe 0 width = 32bytes RX_PW_P0
    write_register(0x11, 32U);

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));

    flush_rx();
    flush_tx();

    // Power up the chip PWR_UP = 1
    write_register(0x00, read_register(0x00) | (1 << 1));
    for(int i = 0; i < 1000000; i++);

    return read_register(0x00) & 0x02;
}

bool switch_tx(uint8_t address[5], uint8_t sizeof_address)
{
    uint8_t dump[5];

    // Set CE low to put chip into the standby mode
    GPIOC->ODR &= ~GPIO_ODR_ODR13;

    // Set TX address TX_ADDR
    cs_enable();
    spi1_send_byte(0x20 | 0x10);
    spi1_buffer_transaction(address, dump, sizeof_address);
    cs_disable();

    // Set RX pipe 0 address RX_ADDR_P0 for auto acknowledgment
    cs_enable();
    spi1_send_byte(0x20 | 0x0A);
    spi1_buffer_transaction(address, dump, sizeof_address);
    cs_disable();

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));

    flush_tx();

    // Disable receiver PRIM_RX = 0
    write_register(0x00, read_register(0x00) & ~(1 << 0));
    for(int i = 0; i < 100000; i++);

    return !(read_register(0x00) & (1 << 0));
}

void transmit(uint8_t *payload, uint8_t sizeof_payload)
{
    uint8_t dump[32];

    cs_enable();
    spi1_send_byte(0xA0);
    spi1_buffer_transaction(payload, dump, sizeof_payload);
    cs_disable();

    // Set CE high to start listening
    GPIOC->ODR |= GPIO_ODR_ODR13;

    // Check any one the bit to be set TX_DS & MAX_RT
    while (!(read_register(0x07) & (1 << 5 | 1 << 4))) {
        for(int i = 0; i < 100000; i++);
    }

    // Set CE low to put chip into the standby mode
    GPIOC->ODR &= ~GPIO_ODR_ODR13;

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));

    flush_tx();
}

bool switch_rx(uint8_t address[5], uint8_t sizeof_address)
{
    uint8_t dump[5];

    // Set CE low to put chip into the standby mode
    GPIOC->ODR &= ~GPIO_ODR_ODR13;

    // Set RX pipe 0 address RX_ADDR_P0 for auto acknowledgment
    cs_enable();
    spi1_send_byte(0x20 | 0x0A);
    spi1_buffer_transaction(address, dump, sizeof_address);
    cs_disable();

    // Set TX address TX_ADDR for auto acknowledgment
    cs_enable();
    spi1_send_byte(0x20 | 0x10);
    spi1_buffer_transaction(address, dump, sizeof_address);
    cs_disable();

    // Enable receiver PRIM_RX = 1
    write_register(0x00, read_register(0x00) | (1 << 0));

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));

    // Set CE high to start listening
    GPIOC->ODR |= GPIO_ODR_ODR13;
    for(int i = 0; i < 1000000; i++);

    return read_register(0x00) & (1 << 0);
}

void receive(uint8_t *buffer, uint8_t sizeof_buffer)
{
    cs_enable();
    spi1_send_byte(0x61);
    spi1_buffer_transaction(buffer, buffer, sizeof_buffer);
    cs_disable();

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));
}

uint8_t *dump_memory(uint8_t registers[], uint8_t num_of_registers, uint8_t data[])
{
    for (uint8_t i = 0; i < (num_of_registers / sizeof(uint8_t)); i++) {
        *(data + i) = read_register(registers[i]);
    }

    return data;
}
