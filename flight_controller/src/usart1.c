#include <stdint.h>
#include "stm32f1xx.h"
#include "usart1.h"

#define USART1_BUFFER_LENGTH 64

static uint8_t usart1_buffer[USART1_BUFFER_LENGTH];
static uint8_t usart1_buffer_counter = 0;
static uint8_t usart1_buffer_write_pointer = 0;
static uint8_t usart1_buffer_read_pointer = 0;

void init_usart1()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;      // enable GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;    // enable USART1 clock
    
    GPIOA->CRH &= ~(GPIO_CRH_CNF9  | GPIO_CRH_MODE9);   // reset PA9
    GPIOA->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10);  // reset PA10
    
    GPIOA->CRH |= GPIO_CRH_MODE9_0;  // 0b01 10MHz output
    GPIOA->CRH &= ~GPIO_CRH_MODE9_1;  // 0b01 10MHz output
    GPIOA->CRH |= GPIO_CRH_CNF9_1;    // PA9: output @ 10MHz - Alt-function Push-pull

    GPIOA->CRH |= GPIO_CRH_CNF10_0;   // PA10 RX - Mode = 0b00 (input) - CNF = 0b01 (input floating)
    
    // configure USART1 registers
    USART1->BRR = 0x34D;        // Mantisa 52 Fraction (16 * 0.83) = 13; (sysClk/(16*baudrate))
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;
    
    // configure NVIC
    NVIC_EnableIRQ(USART1_IRQn);
}

void usart1_write_byte(uint8_t byte)
{
    USART1->DR = byte;

    while (!(USART1->SR & USART_SR_TXE));
}

void usart1_write_string(char *in_string)
{
    while(*in_string) {
        usart1_write_byte((uint8_t) (*in_string));
        in_string++;
    }
}

uint8_t usart1_available(void)
{
    return usart1_buffer_counter;
}

uint8_t usart1_read_byte(void)
{
    uint8_t byte = 0;

    if (usart1_buffer_counter == 0) return byte;

    if (usart1_buffer_read_pointer < USART1_BUFFER_LENGTH) {
        byte = usart1_buffer[usart1_buffer_read_pointer];
        usart1_buffer_read_pointer++;
    } else {
        byte = usart1_buffer[0];
        usart1_buffer_read_pointer = 1;
    }

    usart1_buffer_counter--;
    return byte;
}

void USART1_IRQHandler(void)
{
    // get character from data reg
    uint8_t in_char = USART1->DR;

    if (usart1_buffer_write_pointer < USART1_BUFFER_LENGTH) {
        usart1_buffer[usart1_buffer_write_pointer] = in_char;
        usart1_buffer_write_pointer++;
    } else {
        usart1_buffer[0] = in_char;
        usart1_buffer_write_pointer = 1;
    }

    if (usart1_buffer_counter < USART1_BUFFER_LENGTH) {
        usart1_buffer_counter++;
    }
}