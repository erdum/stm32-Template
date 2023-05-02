#include <stdint.h>
#include "stm32f1xx.h"

void systick_delay_ms(uint16_t delay_ms)
{
	SysTick->LOAD = 72000U;
	SysTick->VAL = 0;
	SysTick->CTRL = (1 << 2U | 1 << 0U);

	for (uint16_t i = 0; i < delay_ms; i++) {

		while (!(SysTick->CTRL & (1 << 16U)));
	}
	SysTick->CTRL = 0;
}
