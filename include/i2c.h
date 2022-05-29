#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

#define I2C_DEBUG_PRINT 0
#define I2C_DUMP_BIN 0

int open_i2c_bus(char *filename, int i2c_addr);

int close_i2c_bus(int file_i2c);

int read_i2c(int file_i2c, uint8_t *buff, int len);

int write_i2c(int file_i2c, uint8_t *buff, int len);


#endif
