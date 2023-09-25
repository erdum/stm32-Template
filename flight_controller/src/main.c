#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "systick.h"
#include "usart1.h"
#include "nrf24.h"
#include "timer2_pwm.h"
#include "timer3_pwm.h"

// Output config
#define output_pwm_period 20
#define throttle_channel 3              // PA2      timer2
#define elevator_channel 4              // PA3      timer2
#define rudder_channel 3                // PB0      timer3

int main(void) 
{
    // Configure device max clock speed 72MHz
    configure_max_clock();
    
    usart1_init();

    timer2_pwm_init(output_pwm_period);
    timer3_pwm_init(output_pwm_period);

    timer2_pwm_enable(throttle_channel);
    timer2_pwm_enable(elevator_channel);
    timer3_pwm_enable(rudder_channel);

    timer2_pwm_write(throttle_channel, 2);
    timer2_pwm_write(elevator_channel, 1.5);
    timer3_pwm_write(rudder_channel, 1.5);

    systick_delay_ms(1000);
    timer2_pwm_write(throttle_channel, 1);

    uint8_t radio_address[5] = { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 };

    trx_init(radio_address, sizeof(radio_address), 108U);
    trx_switch_rx();

    while (1) {

        if (trx_data_available()) {
            uint8_t payload[32];
            trx_receive(payload, sizeof(payload));
            timer2_pwm_write(throttle_channel, 1 + (payload[0] / 100.0));
            timer2_pwm_write(elevator_channel, 1 + (payload[1] / 100.0));
            timer3_pwm_write(rudder_channel, 1 + (payload[2] / 100.0));
        }
    }
    return 0;
}
