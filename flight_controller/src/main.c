#include <stdint.h>
#include "stm32f1xx.h"
#include "clock.h"
#include "timer1_pwm.h"
#include "timer2_pwm.h"
#include "timer3_pwm.h"
#include "timer4_pwm.h"
#include "systick.h"

// Output config
#define output_pwm_period 20
#define throttle_channel 3              // PA2      timer2
#define elevator_channel 4              // PA3      timer2
#define rudder_channel 1                // PA8      timer1
#define ailerons_channel 3              // PB0      timer3
#define flaps_channel 4                 // PB1      timer3
#define extra_channel_1 3               // PB8      timer4
#define extra_channel_2 4               // PB9      timer4

#define POST_delay 500

int main(void) 
{
    // Configure device max clock speed 72MHz
    configure_max_clock();

    // Turn board led on indicating Power-On-Self-Test
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->CRH |= GPIO_CRH_MODE13;

    // Configure timer1 to generate pwm of 20ms on pins PA8 to PA11
    timer1_pwm_init(output_pwm_period);

    // Configure timer2 to generate pwm of 20ms on pins PA0 to PA4
    timer2_pwm_init(output_pwm_period);

    // Configure timer3 to generate pwm of 20ms on pins PA6, PA7, PB0, PB1
    timer3_pwm_init(output_pwm_period);

    // Configure timer4 to generate pwm of 20ms on pins PB6 to PB9
    timer4_pwm_init(output_pwm_period);

    // Enable pwm on all the required pins
    timer1_pwm_enable(rudder_channel);
    timer2_pwm_enable(throttle_channel);
    timer2_pwm_enable(elevator_channel);
    timer3_pwm_enable(ailerons_channel);
    timer3_pwm_enable(flaps_channel);
    timer4_pwm_enable(extra_channel_1);
    timer4_pwm_enable(extra_channel_2);

    // PWM channels Power-On-Self-Test ----------------------------------------
    for (uint8_t counter = 0; counter < 10; counter++) {

        if (counter < 4) {
            timer2_pwm_write(throttle_channel, 2);
        } else {
            timer2_pwm_write(throttle_channel, 1);
        }

        if (counter % 2) {
            timer1_pwm_write(rudder_channel, 1.2);
            timer2_pwm_write(elevator_channel, 1.2);
            timer3_pwm_write(ailerons_channel, 1.2);
            timer3_pwm_write(flaps_channel, 1.2);
            timer4_pwm_write(extra_channel_1, 1.2);
            timer4_pwm_write(extra_channel_2, 1.2);
        } else {
            timer1_pwm_write(rudder_channel, 1.8);
            timer2_pwm_write(elevator_channel, 1.8);
            timer3_pwm_write(ailerons_channel, 1.8);
            timer3_pwm_write(flaps_channel, 1.8);
            timer4_pwm_write(extra_channel_1, 1.8);
            timer4_pwm_write(extra_channel_2, 1.8);
        }

        systick_delay_ms(POST_delay);
    }
    // -------------------------------------------------------------------------

    while (1) {
        // Toggle board led indicating Power-On-Self-Test complete
        GPIOC->ODR ^= GPIO_ODR_ODR13;
        systick_delay_ms(500);
    }
    return 0;
}
