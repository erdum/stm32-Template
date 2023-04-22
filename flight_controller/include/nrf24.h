extern bool init_trx(void);
extern uint8_t read_register(uint8_t reg);
extern bool switch_rx(uint8_t address[5], uint8_t sizeof_address);
extern uint8_t *dump_memory(uint8_t registers[], uint8_t num_of_registers, uint8_t data[]);