#include <stdint.h>
#include "stm32f1xx.h"
#include "spi1.h"
#include "trx.h"

void init_trx(uint8_t mode)
{
    init_spi1();

    // Enable CE pin connected to PA1
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA1 output mode max 50MHz push-pull
    GPIOA->CRL |= GPIO_CRL_MODE1;
    GPIOA->CRL &= ~GPIO_CRL_CNF1;

    // PWR_UP
    uint8_t data[2];
    data[0] = 0b00100000;                           // write command 001AAAAA
    data[1] = 0b00001010 | ((1U << 0U) & mode);     // PWR_UP | PRIM_RX
    spi1_write_buffer(data, sizeof(data));

    for(int i = 0; i < 10000; i++);
}

void transmitte(uint8_t byte)
{
    uint8_t payload[2];

    payload[0] = 0b10100000;                        // write TX payload command 10100000
    payload[1] = byte;                              // 1 byte payload
    spi1_write_buffer(payload, sizeof(payload));

    // CE = 1
    GPIOA->ODR |= GPIO_ODR_ODR1;
}
