void init_i2c1_master(void);
uint8_t i2c1_read_byte(uint8_t device_address);
void i2c1_read_buffer(uint8_t device_address, uint8_t *buffer, uint8_t sizeof_buffer);
void i2c1_write_byte(uint8_t device_address, uint8_t data, uint8_t stop);
