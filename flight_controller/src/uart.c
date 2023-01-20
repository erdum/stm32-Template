#include <stdio.h>
#include "stm32f1xx.h"

static void uart1_send_byte(uint8_t byte)
{
    USART1->DR = byte;

    // Wait for Transmit data register to be empty
    while (!(USART1->SR & USART_SR_TXE));
}

void uart1_init(uint32_t baud_rate)
{
    // Enable APB2 Bus for GPIOA Module
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Reset PA9
    GPIOA->CRH &= ~(GPIO_CRH_MODE9);
    GPIOA->CRH &= ~(GPIO_CRH_CNF9);

    // Set PA9 to Output Mode max 10MHZ
    GPIOA->CRH |= GPIO_CRH_MODE9_0;
    GPIOA->CRH &= ~(GPIO_CRH_MODE9_1);

    // SET PA9 Alternative function output Push Pull
    GPIOA->CRH &= ~(GPIO_CRH_CNF9_0);
    GPIOA->CRH |= GPIO_CRH_CNF9_1;

    // Enable APB2 Bus for USART1 Module
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // Enable USART module and TX
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE;

    SystemCoreClockUpdate();

    uint16_t division = SystemCoreClock / baud_rate;
    USART1->BRR = division;
}


void uart1_write(char *string)
{
    while(*string) {
        uart1_send_byte(*string);
        string++;
    }
}

void uart1_write_int(uint32_t integer)
{
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%ld", integer);
    uart1_write(buffer);
    uart1_write("\n");
}
