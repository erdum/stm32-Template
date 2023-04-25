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

    init_trx();

    while (1) {
        uint8_t data[32];
        cs_enable();
        spi1_send_byte(0x61);
        spi1_buffer_transaction(data, data, sizeof(data));
        cs_disable();

        for (uint8_t i = 0; i < 6; i++) {
            usart1_write_byte(data[i]);
        }

        for(int i = 0; i < 100000; i++);
    }
    
    return 0;
}
