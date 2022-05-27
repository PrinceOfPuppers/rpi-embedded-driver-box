#include "nunchuk.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

#include "driver-box-helpers.h"


int inited = 0;
int calibrated = 0;
int nunchuk_file_i2c;
uint8_t buff[6] = {0};

double joystick_x_cali;
double joystick_y_cali;


void printBin(char a){
    for (int j = 0; j < 8; j++) {
        printf("%d", !!(( a << j) & 0x80));
    }
    printf("\n");
}


int open_i2c_bus(char *filename, int i2c_addr){
    //----- OPEN THE I2C BUS -----
    int file_i2c = open(filename, O_RDWR);
    if (file_i2c < 0)
    {
        //ERROR HANDLING: you can check errno to see what went wrong
        printf("Failed to open the i2c bus\n");
        return -1;
    }

    if (ioctl(file_i2c, I2C_SLAVE, i2c_addr) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        //ERROR HANDLING; you can check errno to see what went wrong
        return -1;
    }

    return file_i2c;
}



int read_i2c(int file_i2c, int len){
    //----- READ BYTES -----
    if (read(file_i2c, buff, len) != len)		//read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
    {
        //ERROR HANDLING: i2c transaction failed
        printf("Failed to read from the i2c bus.\n");
        return 0;
    }
    else
    {
        #if(NUNCHUK_DEBUG)
        for(int i=0; i<len; i++){
            printf("Data[%i]: ", i);
            printBin(buff[i]);
        }
        #endif

        return 1;
    }
}

int write_i2c(int file_i2c, int len){
    if (write(file_i2c, buff, len) != len)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
    {
        printf("Failed to write to the i2c bus.\n");
        return 0;
    }

    #if(NUNCHUK_DEBUG)
    printf("wrote to i2c bus.\n");
    #endif

    return 1;
}


int _get_raw(){
    //buff[0] = 0x00;
    //if(!write_i2c(nunchuk_file_i2c, 1)){return 0;}
    //usleep(10);
    if(!read_i2c(nunchuk_file_i2c, 6)){return 0;}
    return 1;
}

int init_nunchuk(){
    if(!inited){
        char *filename = (char*)"/dev/i2c-1";
        int addr = 0x52;          //<<<<<The I2C address of the slave

        nunchuk_file_i2c = open_i2c_bus(filename, addr);
        if(nunchuk_file_i2c < 0){return 0;}

        buff[0] = 0xf0;
        buff[1] = 0x55;
        if(!write_i2c(nunchuk_file_i2c, 2)){return 0;}

        buff[0] = 0xfb;
        buff[1] = 0x00;
        if(!write_i2c(nunchuk_file_i2c, 2)){return 0;}

        inited = 1;
        usleep(MIN_STARTUP_TIME_MICROSECONDS);
    }

    if(!_get_raw()){return 0;};
    joystick_x_cali = (double)buff[0];
    joystick_y_cali = (double)buff[1];
    calibrated = 1;

    return 1;
}

int get_nunchuk( Nunchuk *n ){
    if(!calibrated){
        if(!init_nunchuk()){return 0;}
    }
    else{
        if(!_get_raw()){return 0;}
    }

    n->joystick_x = max( min( ((double)buff[0] - joystick_x_cali)/96L ,1L ), -1L );
    n->joystick_y = max( min( ((double)buff[1] - joystick_y_cali)/96L ,1L ), -1L );
    n->c = (buff[5] & 0x02) == 0x00;
    n->z = (buff[5] & 0x01) == 0x00;
    return 1;
}

