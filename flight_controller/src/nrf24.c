#include <stdint.h>
#include <stdio.h>
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

static void flush_rx()
{
    cs_enable();
    spi1_send_byte(0xE2);
    cs_disable();
}

static void flush_tx()
{
    cs_enable();
    spi1_send_byte(0xE2);
    cs_disable();
}

void init_trx(void)
{
    // Set CE low to put chip into the standby mode
    GPIOC->ODR &= ~GPIO_ODR_ODR13;

    // Set CRC & CRCO coding scheme to 2 bytes
    write_register(0x00, (1 << 2 | 1 << 3));

    // Enable auto acknowledgment for data all pipes
    write_register(0x01, 0x3F);

    // Enable all data pipes
    write_register(0x02, 0x3F);

    // Set TX/RX address width to 5 bytes, AW = 11
    write_register(0x03, 0x03);

    // Set auto retransmit delay ARD = 0110 = 1500us
    write_register(0x04, read_register(0x04) | (1 << 6 | 1 << 5));

    // Set channel to 76 = 2476MHz
    write_register(0x05, 76U);

    // Set data rate to 1Mbps & PA to 0dbm
    write_register(0x06, (1 << 1 | 1 << 2));

    // Set data pipe 0 width = 32bytes RX_PW_P0
    write_register(0x11, 32U);

    // Set pipe 0 address RX_ADDR_P0
    const uint8_t address[5] = {0xE8E8F0F0E1};
    cs_enable();
    spi1_send_byte(0x20 | 0x0A);
    spi1_buffer_transaction(address, address, sizeof(address));
    cs_disable();

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));

    flush_rx();
    flush_tx();

    // Power up the chip PWR_UP = 1
    write_register(0x00, read_register(0x00) | (1 << 1));
    for(int i = 0; i < 1000000; i++);

    // Set as a receiver PRIM_RX
    write_register(0x00, read_register(0x00) | 0x01);

    // Clear status register bits RX_DR | TX_DS | MAX_RT
    write_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));

    // Set CE to high
    GPIOC->ODR |= GPIO_ODR_ODR13;
}
