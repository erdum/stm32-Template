void init_i2c1_master(void);
void i2c1_read_device(uint8_t device_address, uint16_t device_register_address, uint8_t buffer[], uint8_t num_of_bytes);
void i2c1_write_device(uint8_t device_address, uint16_t device_register_address, uint8_t *data);