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

    const uint8_t imu_address = 0x68;

    while (1) {
        uint8_t power_reg[2] = {0x6B, 0x00};
        i2c1_write_buffer(imu_address, power_reg, sizeof power_reg);

        uint8_t data[2];
        i2c1_write_byte(imu_address, 0x3B, 0U);
        i2c1_read_buffer(imu_address, data, sizeof data);

        char out[20];
        int val = (data[0] << 8U) | data[1];
        sprintf(out, "%u\n", val);
        usart1_write_string(out);

        GPIOC->ODR ^= GPIO_ODR_ODR13;
    }
    return 0;
}
