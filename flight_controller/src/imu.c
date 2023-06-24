#include <stdint.h>
#include "i2c1.h"

#define imu_address 0x68
#define buffer_length 2000

static int32_t accumalatorXa = 0;
static int32_t accumalatorYa = 0;
static int32_t accumalatorZa = 0;
static int32_t accumalatorXg = 0;
static int32_t accumalatorYg = 0;
static int32_t accumalatorZg = 0;
static int16_t bias_error_xa = 0;
static int16_t bias_error_ya = 0;
static int16_t bias_error_za = 0;
static int16_t bias_error_xg = 0;
static int16_t bias_error_yg = 0;
static int16_t bias_error_zg = 0;

void imu_get_raw_data(int16_t *xa, int16_t *ya, int16_t *za, int16_t *xg, int16_t *yg, int16_t *zg, int16_t *temp)
{
    uint8_t power_reg[2] = {0x6B, 0x00};
    i2c1_write_buffer(imu_address, power_reg, sizeof power_reg);

    uint8_t DLPF_reg[2] = {0x1A, 0x04};
    i2c1_write_buffer(imu_address, DLPF_reg, sizeof DLPF_reg);

    uint8_t data[14];
    i2c1_write_byte(imu_address, 0x3B, 0U);
    i2c1_read_buffer(imu_address, data, sizeof data);

    *xa = (data[0] << 8U) | data[1];
    *ya = (data[2] << 8U) | data[3];
    *za = (data[4] << 8U) | data[5];
    *temp = ((int16_t)((data[6] << 8U) | data[7]) / 340) + 36.53;
    *xg = (data[8] << 8U) | data[9];
    *yg = (data[10] << 8U) | data[11];
    *zg = (data[12] << 8U) | data[13];
}

void imu_calibirate(void)
{
    int16_t xa, ya, za, xg, yg, zg, imu_temp;
    imu_get_raw_data(&xa, &ya, &za, &xg, &yg, &zg, &imu_temp);

    for (uint16_t counter = 0; counter < buffer_length; counter++)
    {
        accumalatorXa += xa;
        accumalatorYa += ya;
        accumalatorZa += za;
        accumalatorXg += xg;
        accumalatorYg += yg;
        accumalatorZg += zg;
    }

    bias_error_xa = accumalatorXa / buffer_length;
    bias_error_ya = accumalatorYa / buffer_length;
    bias_error_za = accumalatorZa / buffer_length;
    bias_error_xg = accumalatorXg / buffer_length;
    bias_error_yg = accumalatorYg / buffer_length;
    bias_error_zg = accumalatorZg / buffer_length;

}

void imu_get_values(int16_t *xa, int16_t *ya, int16_t *za, int16_t *xg, int16_t *yg, int16_t *zg)
{
    int16_t XA, YA, ZA, XG, YG, ZG, imu_temp;
    imu_get_raw_data(&XA, &YA, &ZA, &XG, &YG, &ZG, &imu_temp);

    *xa = XA - bias_error_xa;
    *ya = YA - bias_error_ya;
    *za = ZA - bias_error_za;
    *xg = XG - bias_error_xg;
    *yg = YG - bias_error_yg;
    *zg = ZG - bias_error_zg;
}
