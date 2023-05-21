#include <stdint.h>
#include "i2c1.h"

#define imu_address 0x68

void imu_get_raw_acceleration(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t power_reg[2] = {0x6B, 0x00};
    i2c1_write_buffer(imu_address, power_reg, sizeof power_reg);

    uint8_t data[6];
    i2c1_write_byte(imu_address, 0x3B, 0U);
    i2c1_read_buffer(imu_address, data, sizeof data);

    *x = (data[0] << 8U) | data[1];
    *y = (data[2] << 8U) | data[3];
    *z = (data[4] << 8U) | data[5];
}
