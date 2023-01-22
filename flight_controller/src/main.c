#include <stdint.h>
#include "stm32f1xx.h"
#include "uart.h"

int main(void) 
{
    // Necessary for the function of all hardware
    SystemCoreClockUpdate();

    init_usart1();

    while (1) {
        usart1_write_byte('0' + usart1_available());
        usart1_write_byte(usart1_read_byte());
        for(int i = 0; i < 100000; i++);
    }
    
    return 0;
}
