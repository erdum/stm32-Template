#include <stdint.h>
#include "stm32f1xx.h"
#include "i2c.h"

void init_i2c1_master(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;     // enable GPIOB clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;     // enable I2C1 clock

    GPIOB->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_MODE6);    // reset PB6
    GPIOB->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_MODE7);    // reset PB7

    GPIOB->CRL |= GPIO_CRL_MODE6_0;    // 0b01 10MHz output
    GPIOB->CRL |= GPIO_CRL_MODE7_0;    // 0b01 10MHz output

    GPIOB->CRL |= GPIO_CRL_CNF6;       // SCL 0b11 Alt-function Open-drain
    GPIOB->CRL |= GPIO_CRL_CNF7;       // SDA 0b11 Alt-function Open-drain

    I2C1->CR1 |= I2C_CR1_SWRST;        // enter in the Reset mode
    I2C1->CR1 &= ~I2C_CR1_SWRST;       // exit from the Reset mode

    I2C1->CR2 |= I2C_CR2_FREQ_3;       // Peripheral clock speed 8MHz

    I2C1->CCR |= 0x28U;                // I2C bus speed 100KHz

    I2C1->TRISE |= 0X09U;              // Max Rise Time 9ns

    I2C1->CR1 |= I2C_CR1_PE;           // enable Peripheral
}

uint8_t i2c1_read_byte(uint8_t device_address)
{
    volatile int tmp;

    while (I2C1->SR2 & I2C_SR2_BUSY);           // wait for the I2C bus to be free

    I2C1->CR1 |= I2C_CR1_START;                 // send the start condition on the I2C bus
    while (!(I2C1->SR1 & I2C_SR1_SB));          // wait for the start condition to be set

    I2C1->DR = ((device_address << 1U) | 1U);   // send slave device address + read flag
    while (!(I2C1->SR1 & I2C_SR1_ADDR));        // wait for address to be sent
    tmp = I2C1->SR2;                            // clear ADDR flag by reading SR1 -> SR2

    I2C1->CR1 |= I2C_CR1_STOP;                  // send stop condition on the I2C bus

    while (!(I2C1->SR1 & I2C_SR1_RXNE));        // wait for the data
    return I2C1->DR;                            // return received data
}

void i2c1_read_buffer(uint8_t device_address, uint8_t *buffer, uint8_t sizeof_buffer)
{
    volatile int tmp;
    uint8_t counter = (sizeof_buffer / sizeof(uint8_t));    // number of bytes buffer can hold

    I2C1->CR1 |= I2C_CR1_START;                             // send the start condition on the I2C bus
    while (!(I2C1->SR1 & I2C_SR1_SB));                      // wait for the start condition to be set

    I2C1->DR = ((device_address << 1U) | 1U);               // send slave device address + read flag
    while (!(I2C1->SR1 & I2C_SR1_ADDR));                    // wait for address to be sent
    tmp = I2C1->SR2;                                        // clear ADDR flag by reading SR1 -> SR2

    I2C1->CR1 |= I2C_CR1_ACK;                               // enable acknowledge

    while(counter > 0U) {

        if (counter == 1) {
            I2C1->CR1 &= (~I2C_CR1_ACK);                    // disable acknowledge
            I2C1->CR1 |= I2C_CR1_STOP;                      // send stop condition on the I2C bus
        }

        while (!(I2C1->SR1 & I2C_SR1_RXNE));                // wait for the incoming data
        *buffer++ = I2C1->DR;                               // save received data & post increment pointer
        counter--;                                          // decrement number of bytes to received
    }
}

void i2c1_write_byte(uint8_t device_address, uint8_t data, uint8_t stop)
{
    volatile int tmp;

    while (I2C1->SR2 & I2C_SR2_BUSY);           // wait for the I2C bus to be free

    I2C1->CR1 |= I2C_CR1_START;                 // send the start condition on the I2C bus
    while (!(I2C1->SR1 & I2C_SR1_SB));          // wait for the start condition to be set

    I2C1->DR = device_address << 1U;            // send slave device address + write flag
    while (!(I2C1->SR1 & I2C_SR1_ADDR));        // wait for address to be sent
    tmp = I2C1->SR2;                            // clear ADDR flag by reading SR1 -> SR2

    I2C1->DR = data;                            // sending data on the I2C bus
    while (!(I2C1->SR1 & I2C_SR1_TXE));         // wait for the data to be sent

    if (stop & 1U) {
        I2C1->CR1 |= I2C_CR1_STOP;              // send stop condition on the I2C bus
    }
}
