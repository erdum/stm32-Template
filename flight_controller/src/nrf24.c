#include <stdint.h>
#include "stm32f1xx.h"
#include "spi1.h"

static void config_register(uint8_t reg, uint8_t value)
{
    cs_enable();
    spi1_send_byte(0x20 | (0x1F & reg));
    spi1_send_byte(value);
    cs_disable();
}

void init_trx(void)
{
    config_register(0x06, 1 << 5);                                      // Set 250KBPS data rate
    config_register(0x05, 0x2F);                                        // Set channel to 2F 2447 MHz
    config_register(0x02, 0x3F);                                        // Enable all RX pipes
    config_register(0x11, 32U);                                         // RX_PW_P0 = 32 bytes
    config_register(0x12, 32U);                                         // RX_PW_P1 = 32 bytes

    uint8_t address[5];
    address[0] = 0xE8;
    address[1] = 0xE8;
    address[2] = 0xF0;
    address[3] = 0xF0;
    address[4] = 0xE1;

    cs_enable();
    spi1_send_byte(0x20 | 0x0A);
    spi1_buffer_transaction(address, address, sizeof(address));
    cs_disable();

    GPIOC->ODR &= ~GPIO_ODR_ODR13;
    config_register(0x00, (1 << 1) | (1 << 2));
    GPIOC->ODR |= GPIO_ODR_ODR13;

    config_register(0x07, (1 << 6) | (1 << 5) | (1 << 4));
    
    cs_enable();
    spi1_send_byte(0xE2);
    cs_disable();
}
