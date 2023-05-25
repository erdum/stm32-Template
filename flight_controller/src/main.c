#include <stdint.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "usart1.h"

int main(void) 
{
    configure_max_clock();
    usart1_init();

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH |= GPIO_CRH_MODE13;

    usart1_write_string("STM32 Initialized\n");

    while (1) {
        GPIOC->ODR ^= GPIO_ODR_ODR13;
    }
    return 0;
}
