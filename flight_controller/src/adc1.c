#include <stdint.h>
#include "stm32f1xx.h"
#include "adc1.h"
#include "systick.h"
#include "dma.h"

#define ADC_BUFFER_LENGTH 16

static uint16_t ADC_BUFFER[ADC_BUFFER_LENGTH];

void init_adc1(uint8_t channels[], uint8_t sizeof_channels)
{
	init_dma_adc1(ADC_BUFFER, sizeof ADC_BUFFER);

	uint8_t numof_channels = (sizeof_channels / sizeof(uint8_t));
	
	// Enable clock for the GPIOA
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	// Enable clock for the ADC1
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// Stabilization time
	systick_delay_ms(10U);

	// Initialize calibration register
	ADC1->CR2 |= ADC_CR2_RSTCAL;

	// Enable calibration
	ADC1->CR2 |= ADC_CR2_CAL;

	// Wait for the calibration to complete
	while (ADC1->CR2 & ADC_CR2_CAL);

	// Enable ADC1 from sleep
	ADC1->CR2 |= ADC_CR2_ADON;

	// Enable ADC1 continuous mode
	ADC1->CR2 |= ADC_CR2_CONT;

	// Enable ADC1 DMA request
	ADC1->CR2 |= ADC_CR2_DMA;

	// Enable ADC1 scan mode
	ADC1->CR1 |= ADC_CR1_SCAN;

	// Set regular channel sequence length L = 0000 = 1 conversions; L = 1111 = 16 conversions
	ADC1->SQR1 = ((numof_channels - 1U) << ADC_SQR1_L_Pos);

	// Set channels sequence
	for (uint8_t i = 0; i < numof_channels; i++) {

		if (i < 6) {
			ADC1->SQR3 |= (channels[i] << (i * 5));
		} else if (i < 12) {
			ADC1->SQR2 |= (channels[i] << ((i - 30) * 5));
		} else if (i < 16) {
			ADC1->SQR1 |= (channels[i] << ((i - 60) * 5));
		}
	}

	// Start conversion of regular channel
	ADC1->CR2 |= ADC_CR2_SWSTART;

	// Start conversion
	ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t read_adc1(uint8_t channel)
{
	return ADC_BUFFER[channel];
}
