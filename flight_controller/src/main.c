#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "usart1.h"
#include "adc1.h"
#include "timer2_pwm.h"

int main(void) 
{
    configure_max_clock();
    init_usart1();

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH |= GPIO_CRH_MODE13;

    const uint8_t channels[1] = {0};
    init_adc1(channels, sizeof channels);

    // Time-period = 20ms
    timer2_pwm_init(20);
    // Enable timer2 channel 
    timer2_pwm_enable(3);
    timer2_pwm_enable(4);

    usart1_write_string("STM32 Initialized\n");

    timer2_pwm_write(3, 2);
    timer2_pwm_write(4, 1.5);

    systick_delay_ms(5000);

    timer2_pwm_write(3, 1);
    timer2_pwm_write(4, 1.2);

    systick_delay_ms(5000);

    timer2_pwm_write(3, 1.5);
    timer2_pwm_write(4, 1.7);

    while (1) {
        char out[50];
        sprintf(out, "ADC0: %i\n", read_adc1(0));
        usart1_write_string(out);
    }
    return 0;
}
