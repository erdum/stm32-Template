#include <stdint.h>
#include <math.h>
#include "i2c1.h"
#include "systick.h"

#define bmp180_i2c_address 0x77

static int16_t AC1;
static int16_t AC2;
static int16_t AC3;
static uint16_t AC4;
static uint16_t AC5;
static uint16_t AC6;
static int16_t B1;
static int16_t B2;
static int16_t MB;
static int16_t MC;
static int16_t MD;
static double b1,c3,c4,c5,c6,mc,md,x0,x1,x2,y0,y1,y2,p0,p1,p2;

void bmp180_init(void)
{
    uint8_t bmp180_buffer[22];
    i2c1_write_byte(bmp180_i2c_address, 0xAA, 0U);
    i2c1_read_buffer(bmp180_i2c_address, bmp180_buffer, sizeof bmp180_buffer);

    AC1 = (bmp180_buffer[0] << 8) | bmp180_buffer[1];
    AC2 = (bmp180_buffer[2] << 8) | bmp180_buffer[3];
    AC3 = (bmp180_buffer[4] << 8) | bmp180_buffer[5];
    AC4 = (bmp180_buffer[6] << 8) | bmp180_buffer[7];
    AC5 = (bmp180_buffer[8] << 8) | bmp180_buffer[9];
    AC6 = (bmp180_buffer[10] << 8) | bmp180_buffer[11];
    B1 = (bmp180_buffer[12] << 8) | bmp180_buffer[13];
    B2 = (bmp180_buffer[14] << 8) | bmp180_buffer[15];
    MB = (bmp180_buffer[16] << 8) | bmp180_buffer[17];
    MC = (bmp180_buffer[18] << 8) | bmp180_buffer[19];
    MD = (bmp180_buffer[20] << 8) | bmp180_buffer[21];

    // AC1: 8803
    // AC2: -1247
    // AC3: -14435
    // AC4: 34835
    // AC5: 25560
    // AC6: 15901
    // B1: 6515
    // B2: 52
    // MB: -32768
    // MC: -11786
    // MD: 2479

    c3 = 160.0 * pow(2,-15) * AC3;
    c4 = pow(10,-3) * pow(2,-15) * AC4;
    b1 = pow(160,2) * pow(2,-30) * B1;
    c5 = (pow(2,-15) / 160) * AC5;
    c6 = AC6;
    mc = (pow(2,11) / pow(160,2)) * MC;
    md = MD / 160.0;
    x0 = AC1;
    x1 = 160.0 * pow(2,-13) * AC2;
    x2 = pow(160,2) * pow(2,-25) * B2;
    y0 = c4 * pow(2,15);
    y1 = c4 * c3;
    y2 = c4 * b1;
    p0 = (3791.0 - 8.0) / 1600.0;
    p1 = 1.0 - 7357.0 * pow(2,-20);
    p2 = 3038.0 * 100.0 * pow(2,-36);
}

double bmp180_get_altitude(uint16_t pressure_msl)
{
    // Write 0x2E at address 0xF4 for raw temp readout
    uint8_t regsUT[2] = {0xF4, 0x2E};
    i2c1_write_buffer(bmp180_i2c_address, regsUT, sizeof regsUT);
    systick_delay_ms(5);

    // Read MSB & LSB
    uint8_t dataUT[2];
    i2c1_write_byte(bmp180_i2c_address, 0xF6, 0);
    i2c1_read_buffer(bmp180_i2c_address, dataUT, sizeof dataUT);

    // // Write 0x34 at address 0xF4 for raw pressure readout
    uint8_t regsUP[2] = {0xF4, 0x34};
    i2c1_write_buffer(bmp180_i2c_address, regsUP, sizeof regsUP);
    systick_delay_ms(8);

    // Read MSB & LSB
    uint8_t dataUP[3];
    i2c1_write_byte(bmp180_i2c_address, 0xF6, 0);
    i2c1_read_buffer(bmp180_i2c_address, dataUP, sizeof dataUP);

    double tu, a, T;
    tu = (dataUT[0] * 256.0) + dataUT[1];
    a = c5 * (tu - c6);
    T = a + (mc / (a + md));

    double pu,s,x,y,z;
    pu = (dataUP[0] * 256.0) + dataUP[1] + (dataUP[2] / 256.0);
    s = T - 25.0;
    x = (x2 * pow(s, 2)) + (x1 * s) + x0;
    y = (y2 * pow(s, 2)) + (y1 * s) + y0;
    z = (pu - x) / y;
    double P = (p2 * pow(z, 2)) + (p1 * z) + p0;
    double altitude = 44330.0 * (1 - ( pow(P / pressure_msl, 1.0 / 5.255)));

    return altitude;
}
