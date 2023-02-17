#include <stdint.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "usart1.h"
#include "i2c1.h"
#include "spi1.h"

#define DEVICE 0x68

int main(void) 
{
    // Necessary for the function of all hardware
    SystemCoreClockUpdate();

    init_usart1();
    // init_i2c1_master();
    init_spi1();
    // init_trx(0U);

    // RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    // GPIOC->CRH |= GPIO_CRH_MODE13;
    // GPIOC->CRH &= ~GPIO_CRH_CNF13;

    // usart1_write_string("STM32 Initialized\n");
    
    while (1) {
        // uint8_t pwr_reg[2];
        // pwr_reg[0] = 107U;
        // pwr_reg[1] = 0U;
        // i2c1_write_buffer(DEVICE, pwr_reg, sizeof(pwr_reg));
        // for(int i = 0; i < 100000; i++);

        // uint8_t data[4];
        // i2c1_write_byte(DEVICE, 63U, 0U);
        // i2c1_read_buffer(DEVICE, data, sizeof(data));

        // int16_t raw_acc_z = ((data[0] << 8U) | data[1]);
        // char str[10];
        // sprintf(str, "%u", raw_acc_z);
        // usart1_write_string(str);
        // usart1_write_string("\n");

        // transmitte(107U);
        // GPIOC->ODR ^= GPIO_ODR_ODR13;

        uint8_t data[6];
        uint8_t load[6];
        uint8_t counter = 0;
        data[0] = 'E';
        data[1] = 'R';
        data[2] = 'D';
        data[3] = 'U';
        data[4] = 'M';
        data[5] = '\n';
        spi1_buffer_transaction(data, load, sizeof(data));

        while (counter <= 5) {
            usart1_write_byte(load[counter]);
            counter++;
        }
        for(int i = 0; i < 100000; i++);
    }
    
    return 0;
}
