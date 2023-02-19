#include <stdint.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "usart1.h"
#include "spi1.h"

int main(void) 
{
    // Necessary for the function of all hardware
    SystemCoreClockUpdate();

    // System initialization
    init_usart1();
    init_spi1();

    usart1_write_string("STM32 Initialized\n");

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // Initialize PA3 for CE line
    GPIOA->CRL |= GPIO_CRL_MODE3;                   // 0b11 output max 50MHz
    GPIOA->CRL = ~GPIO_CRL_CNF3;                    // 0b00 output push-pull
    
    GPIOC->CRH |= GPIO_CRH_MODE13;                   // 0b11 output max 50MHz
    GPIOC->CRH = ~GPIO_CRH_CNF13;                    // 0b00 output push-pull

    // Reset PA3 & PC13
    GPIOA->ODR &= ~GPIO_ODR_ODR3;
    GPIOC->ODR &= ~GPIO_ODR_ODR13;

    for(int i = 0; i < 100000; i++);

    uint8_t status[2];
    uint8_t command[2];

    command[0] = 0b00100000;                        // write config register
    command[1] = 0b00000011;                        // PWR_UP | PRIM_RX
    spi1_buffer_transaction(command, status, sizeof(command));

    for(int i = 0; i < 100000; i++);

    command[0] = 0b00100000 | 0x02;                 // write EN_RXADDR
    command[1] = 0b00000001;                        // enable pipe 0 address
    spi1_buffer_transaction(command, status, sizeof(command));

    uint8_t pipe_address[6];
    uint8_t pipe_status[6];
    pipe_address[0] = 0b00100000 | 0x0A;            // write RX_ADDR_P0 => '00001'
    pipe_address[1] = 0x30;                         // write 0
    pipe_address[2] = 0x30;                         // write 0
    pipe_address[3] = 0x30;                         // write 0
    pipe_address[4] = 0x30;                         // write 0
    pipe_address[5] = 0x31;                         // write 1
    spi1_buffer_transaction(pipe_address, pipe_status, sizeof(pipe_address));

    for(int i = 0; i < 100000; i++);

    GPIOA->ODR |= GPIO_ODR_ODR3;
    GPIOC->ODR |= GPIO_ODR_ODR13;

    for(int i = 0; i < 100000; i++);

    command[0] = 0b11111111;                        // read status register NOP
    command[1] = 0b11111111;                        // read status register NOP
    char output[10];

    while (1) {
        for (uint8_t counter = 0; counter <= 2; counter++) {
            spi1_buffer_transaction(command, status, sizeof(command));
            sprintf(output, "%u", status[counter]);
            usart1_write_string(output);
        }

        for(int i = 0; i < 100000; i++);
    }
    
    return 0;
}
