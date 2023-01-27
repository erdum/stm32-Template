#include <stdint.h>
#include "stm32f1xx.h"
#include "uart.h"
#include "i2c.h"

int main(void) 
{
    // Necessary for the function of all hardware
    SystemCoreClockUpdate();

    init_usart1();
    init_i2c1_master();

    usart1_write_string("STM32 Initialized\n");
    
    while (1) {
        uint8_t str[5];
        i2c1_read_buffer(0x08, str, sizeof(str));

        for (uint8_t i = 0; i < (sizeof(str) / sizeof(uint8_t)); i++) {
            usart1_write_byte(str[i]);
        }
        usart1_write_string("\n");

        for(int i = 0; i < 100000; i++);
    }
    
    return 0;
}
