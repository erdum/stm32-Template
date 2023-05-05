extern void i2c1_master_init(void);
extern uint8_t i2c1_read_byte(uint8_t device_address);
extern void i2c1_read_buffer(uint8_t device_address, uint8_t *buffer, uint8_t sizeof_buffer);
extern void i2c1_write_byte(uint8_t device_address, uint8_t data, uint8_t stop);
extern void i2c1_write_buffer(uint8_t device_address, uint8_t *buffer, uint8_t sizeof_buffer);
