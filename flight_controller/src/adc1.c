#include <stdint.h>
#include "stm32f1xx.h"

void init_adc1(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;                                 // enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;                                 // enable ADC1 clock
}
