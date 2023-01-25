void init_i2c1_master(void);
uint8_t i2c1_read_byte(uint8_t device_address);
void i2c1_write_byte(uint8_t device_address, uint8_t data);
void i2c1_send_stop(void);