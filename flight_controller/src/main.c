#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "usart1.h"
#include "adc1.h"

int main(void) 
{
    configure_max_clock();
    init_usart1();

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH |= GPIO_CRH_MODE13;

    const uint8_t channels[2] = {0, 1};
    init_adc1(channels, sizeof channels);

    usart1_write_string("STM32 Initialized\n");

    while (1) {
        char out[40];
        sprintf(out, "ADC0: %i, ADC1: %i\n", read_adc1(0), read_adc1(1));
        usart1_write_string(out);
    }
    return 0;
}
