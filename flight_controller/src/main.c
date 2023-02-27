#include <stdint.h>
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

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // // Initialize PA3 & PC13 for CE line
    GPIOA->CRL |= GPIO_CRL_MODE3;                   // 0b11 output max 50MHz
    GPIOA->CRL &= ~GPIO_CRL_CNF3;                    // 0b00 output push-pull
    GPIOC->CRH |= GPIO_CRH_MODE13;                  // 0b11 output max 50MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13;                   // 0b00 output push-pull

    // // Reset PA3 & PC13
    GPIOA->ODR &= ~GPIO_ODR_ODR3;
    GPIOC->ODR &= ~GPIO_ODR_ODR13;

    for(int i = 0; i < 1000000; i++);

    trx_rx_up();

    while (1) {

        GPIOC->ODR ^= GPIO_ODR_ODR13;
        for(int i = 0; i < 100000; i++);
    }
    
    return 0;
}
