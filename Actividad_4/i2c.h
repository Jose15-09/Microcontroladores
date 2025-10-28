#ifndef _i2c
#define _i2c
#include <stdint.h>
#define I2C_FASTMODE 0x2D
#define I2C_STDMODE 0xB4

void init_i2c();
void start_i2c(void);
void stop_i2c(void);
void data_i2c(uint8_t data);
void adress_i2c(uint8_t address, uint8_t r_w);
void write_i2c(uint8_t address, uint8_t data[], uint64_t length);
#endif