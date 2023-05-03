#include <stdint.h>
#include "stm32f1xx.h"
#include "dma.h"

void init_dma_adc1(uint16_t *buffer_address, uint16_t sizeof_buffer)
{
	// Enable DMA1 controller clock
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	// Destination memory size 16bits
	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;

	// Source peripheral size 16bits
	DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;

	// Enable memory post increment
	DMA1_Channel1->CCR |= DMA_CCR_MINC;

	// Enable circular mode
	DMA1_Channel1->CCR |= DMA_CCR_CIRC;

	// Define the number of bytes should be written to the buffer
	DMA1_Channel1->CNDTR = (sizeof_buffer / sizeof(uint16_t));

	// Set the peripheral address
	DMA1_Channel1->CPAR = &ADC1->DR;

	// Set the memory address
	DMA1_Channel1->CMAR = buffer_address;

	// Enable DMA channel1
	DMA1_Channel1->CCR |= DMA_CCR_EN;
}
