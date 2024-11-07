#include <stdint.h>
#include "stm32f1xx.h"
#include "timer2_pwm.h"

void timer2_pwm_init(uint16_t time_period_ms)
{
	// Enable clock for Timer2
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// Enable clock for GPIOA
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	// Set pre-scaler to 7200 TIM2 frequency = 10000, APB1 prescaler is 2 so APB1CLK x 2 = TIMxCLK = 36MHz x 2
	TIM2->PSC = 7200 - 1;

	// Set auto-reload value to form PWM time period, 10000 = 1s
	TIM2->ARR = (time_period_ms * 10) - 1;

	// Set all channel output mode to PWM 1
	TIM2->CCMR2 |= (
		TIM_CCMR2_OC4M_2
		| TIM_CCMR2_OC4M_1
		| TIM_CCMR2_OC3M_2
		| TIM_CCMR2_OC3M_1
	);

	// Set all channel output mode to PWM 1
	TIM2->CCMR1 |= (
		TIM_CCMR1_OC2M_2
		| TIM_CCMR1_OC2M_1
		| TIM_CCMR1_OC1M_2
		| TIM_CCMR1_OC1M_1
	);

	// Enable all channels compare output
	TIM2->CCER |= (TIM_CCER_CC4E | TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E);

	// Clear timer
	TIM2->CNT = 0;

	// Enable Timer2
	TIM2->CR1 = TIM_CR1_CEN;
}

void timer2_pwm_enable(uint8_t channel)
{
	// Channel could be in the range of 1 to 4;

	// Reset channel pin PA0 to PA4
	GPIOA->CRL &= ~(0xF << ((channel - 1) * 4));

	// Configure channel pin to alt-output push-pull 50MHz
	GPIOA->CRL |= (0x3 << ((channel - 1) * 4));
	GPIOA->CRL |= (0x2 << (((channel - 1) * 4) + 2));
}

void timer2_pwm_write(uint8_t channel, float on_time)
{
	// Write values in 1000s to get high resolution
	uint16_t compare_value = (on_time * 10) - 1;

	if (channel == 1)
	{
		TIM2->CCR1 = compare_value;
	}
	else if (channel == 2)
	{
		TIM2->CCR2 = compare_value;
	}
	else if (channel == 3)
	{
		TIM2->CCR3 = compare_value;
	}
	else if (channel == 4)
	{
		TIM2->CCR4 = compare_value;
	}
}
