#include <stdint.h>
#include "stm32f1xx.h"
#include "spi1.h"
#include "usart1.h"

void init_radio(void)
{
    init_spi1();

    // Initialize PA3 for CE line
    GPIOA->CRL |= GPIO_CRL_MODE3;                   // 0b11 output max 50MHz
    GPIOA->CRL = ~GPIO_CRL_CNF3;                    // 0b00 output push-pull

    // Reset PA3 
    GPIOA->ODR &= ~GPIO_ODR_ODR4;

    // Power up
    uint8_t status[2];
    uint8_t command[2];
    command[0] = 0b00100000;                        // Write register command 001AAAAA
    command[1] = 0b00000010;                        // PWR_UP bit 1

    spi1_buffer_transaction(command, status, sizeof(command));

    for (uint8_t counter = 0; counter <= 2; counter++) {
        usart1_write_byte(status[counter]);
        usart1_write_byte(0x0A);                    // line break 0x0A = \n
    }
}
