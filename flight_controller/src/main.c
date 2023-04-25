#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "usart1.h"
#include "spi1.h"
#include "nrf24.h"

int main(void) 
{
    // Necessary for the function of all hardware
    SystemCoreClockUpdate();

    // System initialization
    init_usart1();
    init_spi1();

    usart1_write_string("STM32 Initialized\n");

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // // Initialize PC13 for CE line
    GPIOC->CRH |= GPIO_CRH_MODE13;                  // 0b11 output max 50MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13;                   // 0b00 output push-pull

    const uint8_t address[5] = {
        0xE8,
        0xE8,
        0xF0,
        0xF0,
        0xE1
    };

    init_trx();
    switch_rx(address, sizeof address);

    while (1) {
        uint8_t data[32];

        if (trx_data_available()) {
            cs_enable();
            spi1_send_byte(0x61);
            spi1_buffer_transaction(data, data, sizeof data);
            cs_disable();
            usart1_write_string(data);
        }
    }
    
    return 0;
}
