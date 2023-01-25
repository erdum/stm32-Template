#include <stdint.h>
#include <stdio.h>
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
        i2c1_write_byte(0x08, 0xFF);
        i2c1_send_stop();

        // char str[4];
        // sprintf(str, "%d", data);
        // usart1_write_string(str);
        // usart1_write_string("\n");

        for(int i = 0; i < 100000; i++);
    }
    
    return 0;
}
