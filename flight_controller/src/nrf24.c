#include <stdint.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "spi1.h"
#include "usart1.h"

static void config_register(uint8_t reg, uint8_t value)
{
    cs_enable();
    spi1_send_byte(0x20 | (0x1F & reg));
    spi1_send_byte(value);
    cs_disable();
}

static void flush(void)
{
    cs_enable();
    spi1_send_byte(0xE1);
    cs_disable();

    cs_enable();
    spi1_send_byte(0xE2);
    cs_disable();
}

static void write_register(uint8_t reg, uint8_t *data, uint8_t data_len)
{
    cs_enable();
    spi1_send_byte(0x20 | (0x1F & reg));
    spi1_buffer_transaction(data, data, data_len);
    cs_disable();
}

void read_register(uint8_t reg, uint8_t *buffer, uint8_t buffer_len)
{
    cs_enable();
    spi1_send_byte(0x00 | (0x1F & reg));
    spi1_buffer_transaction(buffer, buffer, buffer_len);
    cs_disable();
}

void init_trx(void)
{
    // Only pipe 0 is enabled for the receiving with address of 0xE7E7E7E7E7
    // TX address is 0xE7E7E7E7E7

    config_register(0x05, 0x00);                                        // Set channel to 000 2400 MHz
    config_register(0x02, 0x01);                                        // Enable RX address pipe 0
    config_register(0x11, 32U);                                         // RX_PW_P0 = 32 bytes

    // Power up procedure
    GPIOA->ODR &= ~GPIO_ODR_ODR3;                                       // Pull CE pin down
    config_register(0x00, ((1 << 1) | (1 << 2)));                       // Set config register PWR_UP | PRIM_RX
    GPIOA->ODR |= GPIO_ODR_ODR3;                                        // Pull CE pin high

    for(int i = 0; i < 100000; i++);

    config_register(0x07, ((1 << 6) | (1 << 5) | (1 << 4)));            // Clear status register
    flush();                                                            // Flush FIFO's
}

void dump_trx_status(void)
{
    cs_enable();
    uint8_t status = spi1_send_byte(0xFF);                              // NOP
    cs_disable();

    if (status & (1 << 6)) {
        usart1_write_string("RX data arrived |");
    }

    if (status & (1 << 5)) {
        usart1_write_string("TX data sent |");
    }

    if ((status & 0x0E) == 0x0E) {
        usart1_write_string("RX FIFO empty\n");
    } else if ((status & 0x0E) == 0xC) {
        usart1_write_string("Data pipe not used\n");
    } else {
        char out[3];
        uint8_t data_pipe = status & 0x0E;
        data_pipe = data_pipe >> 1;
        sprintf(out, "%u\n", data_pipe);
        usart1_write_string(out);
    }
}
