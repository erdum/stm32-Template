#include <stdint.h>
#include "stm32f1xx.h"
#include "timer1_pwm.h"

void timer1_pwm_init(uint16_t time_period_ms)
{
	// Enable clock for Timer1
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	// Enable clock for GPIOA
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	// Set pre-scaler to 7200 TIM1 frequency = 10000, APB2 prescaler is 1 so APB2CLK x 1 = TIMxCLK = 72MHz x 1
	TIM1->PSC = 7200 - 1;

	// Set auto-reload value to form PWM time period, 10000 = 1s
	TIM1->ARR = (time_period_ms * 10) - 1;

	// Set all channel output mode to PWM 1
	TIM1->CCMR2 |= (
		TIM_CCMR2_OC4M_2
		| TIM_CCMR2_OC4M_1
		| TIM_CCMR2_OC3M_2
		| TIM_CCMR2_OC3M_1
	);

	// Set all channel output mode to PWM 1
	TIM1->CCMR1 |= (
		TIM_CCMR1_OC2M_2
		| TIM_CCMR1_OC2M_1
		| TIM_CCMR1_OC1M_2
		| TIM_CCMR1_OC1M_1
	);

	// Enable all channels compare output
	TIM1->CCER |= (TIM_CCER_CC4E | TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E);

	// Clear timer
	TIM1->CNT = 0;

	// Enable Timer1
	TIM1->CR1 = TIM_CR1_CEN;
}

void timer1_pwm_enable(uint8_t channel)
{
	// Channel could be in the range of 1 to 4;

	// Reset channel pin PA8 to PA11
	GPIOA->CRH &= ~(0xF << ((channel - 1) * 4));

	// Configure channel pin to alt-output push-pull 50MHz
	GPIOA->CRH |= (0x3 << ((channel - 1) * 4));
	GPIOA->CRH |= (0x2 << (((channel - 1) * 4) + 2));
}

void timer1_pwm_write(uint8_t channel, float on_time)
{
	uint16_t compare_value = (on_time * 10) - 1;

	if (channel == 1)
	{
		TIM1->CCR1 = compare_value;
	}
	else if (channel == 2)
	{
		TIM1->CCR2 = compare_value;
	}
	else if (channel == 3)
	{
		TIM1->CCR3 = compare_value;
	}
	else if (channel == 4)
	{
		TIM1->CCR4 = compare_value;
	}
}
