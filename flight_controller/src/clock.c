#include <stdint.h>
#include "stm32f1xx.h"
#include "clock.h"

void configure_clock(void)
{
	// Enable power interface
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	
	// Enable HSE
	RCC->CR |= RCC_CR_HSEON;

	// Wait for HSE to become stable
	while (!(RCC->CR & RCC_CR_HSERDY));

	// Enable pre-fetch buffer
	FLASH->ACR |= FLASH_ACR_PRFTBE;
	
	// Set flash latency to (2 wait states)
	FLASH->ACR &= ~(FLASH_ACR_LATENCY_2 | FLASH_ACR_LATENCY_0);
	FLASH->ACR |= FLASH_ACR_LATENCY_1;

	// Disable PLL input divider
	RCC->CFGR |= RCC_CFGR_PLLXTPRE_HSE;

	// Select HSE as PLL input HSE = 8MHz
	RCC->CFGR |= RCC_CFGR_PLLSRC;

	// Set PLL multiplier to x9 PLLMUL = 0111, 8MHz x 9 = 72MHz
	RCC->CFGR |= (RCC_CFGR_PLLMULL_2 | RCC_CFGR_PLLMULL_1 | RCC_CFGR_PLLMULL_2);

	// Divide SYSCLK / 2 for APB1 = 72MHz / 2 = 36MHz
	RCC->CFGR |= RCC_CFGR_PPRE1_2;

	// Divide APB2 / 2 for ADCCLK because its max speed is 14MHz
	RCC->CFGR |= RCC_CFGR_ADCPRE_1;

	// Enable PLL
	RCC->CR |= RCC_CR_PLLON;

	// Wait for PLL to become stable
	while (!(RCC->CR & RCC_CR_PLLRDY));

	// Switch SYSCLK to PLL
	RCC->CR |= RCC_CFGR_SW_PLL;

	// Wait for SYSCLK to switch
	while (!(RCC->CR & RCC_CFGR_SWS_PLL));
}
