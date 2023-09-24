#include <stdint.h>
#include "stm32f1xx.h"
#include "spi1.h"

void spi1_master_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);         // reset PA4  MODE 0b00
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);         // reset PA5  MODE 0b00
    GPIOA->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);         // reset PA6  MODE 0b00
    GPIOA->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);         // reset PA7  MODE 0b00

    GPIOA->CRL |= GPIO_CRL_MODE4;                           // SS 0b11 50MHz output push-pull
    GPIOA->CRL |= GPIO_CRL_MODE5_0 | GPIO_CRL_CNF5_1;       // SCK 0b01 10MHz output alt push-pull
    GPIOA->CRL |= GPIO_CRL_CNF6_0;                          // MISO 0b01 Input floating
    GPIOA->CRL |= GPIO_CRL_MODE7_0 | GPIO_CRL_CNF7_1;       // MOSI 0b01 10MHz output alt push-pull

    SPI1->CR1 |= SPI_CR1_MSTR;                              // master configuration
    SPI1->CR1 |= SPI_CR1_BR_1;                              // baud rate clk / 8 = 72MHz / 8 = 9MHz
    SPI1->CR1 &= ~(SPI_CR1_RXONLY);                         // full duplex
    SPI1->CR1 &= ~(SPI_CR1_LSBFIRST);                       // MSB first
    SPI1->CR1 &= ~(SPI_CR1_CPOL);                           // clock 0 when idle
    SPI1->CR1 &= ~(SPI_CR1_CPHA);                           // data sampled on rising edge
    SPI1->CR1 &= ~(SPI_CR1_DFF);                            // 8 bit data frame format
    SPI1->CR1 &= ~(SPI_CR1_BIDIMODE);                       // 2-line unidirectional data transfer mode
    SPI1->CR1 |= SPI_CR1_SSM;                               // software slave management enable
    SPI1->CR1 |= SPI_CR1_SSI;                               // enable internal slave select
    SPI1->CR1 |= SPI_CR1_SPE;
}

void spi1_buffer_transaction(
    uint8_t *tx_buffer,
    uint8_t *rx_buffer,
    uint8_t sizeof_buffer
) {
    uint8_t counter = (sizeof_buffer / sizeof(uint8_t));

    while (counter > 0U) {
        *rx_buffer = spi1_send_byte(*tx_buffer);

        tx_buffer++;
        rx_buffer++;
        counter--;
    }

    while (SPI1->SR & SPI_SR_BSY);
}

uint8_t spi1_send_byte(uint8_t data)
{
    uint8_t buffer;
    
    SPI1->DR = data;
    
    while (!(SPI1->SR & SPI_SR_RXNE));
    buffer = SPI1->DR;

    while(!(SPI1->SR & SPI_SR_TXE));

    return buffer;
}

void cs_enable(void)
{
    // CS Enable
    GPIOA->ODR &= ~(GPIO_ODR_ODR4);
}

void cs_disable(void)
{
    // CS Disable
    GPIOA->ODR |= GPIO_ODR_ODR4;

    for(int i = 0; i < 10; i++);
}
