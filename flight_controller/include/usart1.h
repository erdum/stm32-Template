extern void usart1_init();
extern void usart1_write_byte(uint8_t byte);
extern void usart1_write_string(char *in_string);
extern uint8_t usart1_available(void);
extern uint8_t usart1_read_byte(void);
extern void print_float(double value, uint8_t precision);
