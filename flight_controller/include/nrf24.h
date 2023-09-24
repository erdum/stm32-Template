extern bool trx_init(uint8_t address[5], uint8_t sizeof_address, uint8_t channel);
extern bool trx_switch_rx(void);
extern bool trx_switch_tx(void);
extern void trx_receive(uint8_t *buffer, uint8_t sizeof_buffer);
extern void trx_transmit(uint8_t *payload, uint8_t sizeof_payload);
extern bool trx_data_available(void);
extern void trx_dump_memory(char out[]);
