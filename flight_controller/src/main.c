#include <stdint.h>
#include <stdio.h>
// #include <stdbool.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "systick.h"
#include "usart1.h"
#include "i2c1.h"
#include "imu.h"
// #include "nrf24.h"
// #include "timer2_pwm.h"
// #include "timer3_pwm.h"
// #include "adc1.h"

// Output config
// #define output_pwm_period 20
// #define throttle_channel 3              // PA2      timer2
// #define elevator_channel 4              // PA3      timer2
// #define rudder_channel 3                // PB0      timer3
// #define battery_voltage_pin 0           // PA0      ADC1 0
// #define current_pin 1                   // PA1      ADC1 1

// #define battery_voltage_multiplier 11
// #define current_multiplier 2

int main(void) 
{
    // Configure device max clock speed 72MHz
    configure_max_clock();
    
    usart1_init();
    i2c1_master_init();

    // timer2_pwm_init(output_pwm_period);
    // timer3_pwm_init(output_pwm_period);

    // timer2_pwm_enable(throttle_channel);
    // timer2_pwm_enable(elevator_channel);
    // timer3_pwm_enable(rudder_channel);

    // timer2_pwm_write(throttle_channel, 2);
    // timer2_pwm_write(elevator_channel, 1.5);
    // timer3_pwm_write(rudder_channel, 1.5);

    // systick_delay_ms(1000);
    // timer2_pwm_write(throttle_channel, 1);

    // uint8_t radio_address[5] = { 0xE7, 0xE7, 0xE7, 0xE7, 0xE7 };

    // trx_init(radio_address, sizeof(radio_address), 108U);
    // trx_switch_rx();

    // uint8_t adc1_channels[] = { battery_voltage_pin, current_pin };
    // adc1_init(adc1_channels, sizeof(adc1_channels));

    while (1) {

        // if (trx_data_available()) {
        //     uint8_t payload[32];
        //     trx_receive(payload, sizeof(payload));
        //     timer2_pwm_write(throttle_channel, 1 + (payload[0] / 100.0));
        //     timer2_pwm_write(elevator_channel, 1 + (payload[1] / 100.0));
        //     timer3_pwm_write(rudder_channel, 1 + (payload[2] / 100.0));
        // }

        // uint16_t raw_battery_voltage = adc1_read(battery_voltage_pin);
        // uint16_t raw_current = adc1_read(current_pin);

        // float voltage = raw_battery_voltage / (4096.0 / 3.285);

        int16_t ax, ay, az, gx, gy, gz, temp;
        imu_get_raw_data(&ax, &ay, &az, &gx, &gy, &gz, &temp);

        // float test = (ax / 32768.0) * 2.0;

        char out[40];
        sprintf(out, "%d\n", ax);
        usart1_write_string(out);
    }
    return 0;
}
