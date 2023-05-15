#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "usart1.h"
#include "i2c1.h"

int main(void) 
{
    configure_max_clock();
    usart1_init();
    i2c1_master_init();

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH |= GPIO_CRH_MODE13;

    usart1_write_string("STM32 Initialized\n");

    const uint8_t barometer_address = 0x77;

    while (1) {
        uint8_t regs[2] = {0xF4, 0x2E};
        i2c1_write_buffer(barometer_address, regs, sizeof regs);
        systick_delay_ms(5);

        uint8_t data[2];
        i2c1_write_byte(barometer_address, 0xF6, 0U);
        i2c1_read_buffer(barometer_address, data, sizeof data);

        char out[20];
        int16_t value = data[0] << 8U | data[1];
        sprintf(out, "%d\n", value);
        usart1_write_string(out);

        GPIOC->ODR ^= GPIO_ODR_ODR13;
    }
    return 0;
}
