extern void spi1_master_init(void);
extern void spi1_buffer_transaction(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t sizeof_buffer);
extern uint8_t spi1_send_byte(uint8_t data);
extern void cs_enable(void);
extern void cs_disable(void);
