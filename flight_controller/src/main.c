#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "usart1.h"
#include "clock.h"

int main(void) 
{
    configure_clock();

    init_usart1();
    usart1_write_string("STM32 Initialized\n");

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH |= GPIO_CRH_MODE13;                      // 0b11 output max 50MHz

    while (1) {
        char out[6];
        sprintf(out, "0x%X\n", 0xEE);
        usart1_write_string(out);
    }
    
    return 0;
}
