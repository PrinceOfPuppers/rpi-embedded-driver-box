#include "i2c.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "driver-box-helpers.h"

int open_i2c_bus(char *filename, int i2c_addr){
    //----- OPEN THE I2C BUS -----
    int file_i2c = open(filename, O_RDWR);
    if (file_i2c < 0)
    {
        #if(I2C_DEBUG_PRINT)
        printf("i2c: failed to open bus\n");
        #endif
        return -1;
    }

    if (ioctl(file_i2c, I2C_SLAVE, i2c_addr) < 0)
    {
        #if(I2C_DEBUG_PRINT)
        printf("i2c: failed to talk to device\n");
        #endif
        return -1;
    }

    return file_i2c;
}

int close_i2c_bus(int file_i2c){
    return close(file_i2c);

}



int read_i2c(int file_i2c, uint8_t *buff, int len){
    if (read(file_i2c, buff, len) != len)
    {
        #if(I2C_DEBUG_PRINT)
        printf("i2c: failed to read bus\n");
        #endif
        return 0;
    }
    #if(I2C_DUMP_BIN)
    printf("I2C Read:", i);
    for(int i=0; i<len; i++){
        printf("Data[%i]: ", i);
        printBin(buff[i]);
    }
    #endif

    return 1;
}

int write_i2c(int file_i2c, uint8_t *buff, int len){
    if (write(file_i2c, buff, len) != len)
    {
        #if(I2C_DEBUG_PRINT)
        printf("Failed to write to the i2c bus.\n");
        #endif
        return 0;
    }

    #if(I2C_DUMP_BIN)
    printf("I2C Write:", i);
    for(int i=0; i<len; i++){
        printf("Data[%i]: ", i);
        printBin(buff[i]);
    }
    #endif

    #if(I2C_DEBUG_PRINT)
    printf("wrote to i2c bus.\n");
    #endif

    return 1;
}

