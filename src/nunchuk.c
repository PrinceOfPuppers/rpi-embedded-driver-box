#include "nunchuk.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#include "driver-box-helpers.h"
#include "i2c.h"


int inited = 0;
int calibrated = 0;
int nunchuk_file_i2c;

#define NUNCHUK_PACKET_SIZE 6
uint8_t buff[NUNCHUK_PACKET_SIZE] = {0};

double joystick_x_cali;
double joystick_y_cali;

int _get_raw(){
    buff[0] = 0x00;
    if(!write_i2c(nunchuk_file_i2c, buff, 1)){return 0;}
    usleep(200);
    if(!read_i2c(nunchuk_file_i2c, buff, NUNCHUK_PACKET_SIZE)){return 0;}
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
        if(!write_i2c(nunchuk_file_i2c, buff, 2)){return 0;}

        buff[0] = 0xfb;
        buff[1] = 0x00;
        if(!write_i2c(nunchuk_file_i2c, buff, 2)){return 0;}

        inited = 1;
        usleep(NUNCHUK_MIN_STARTUP_TIME_MICROSECONDS);
    }

    if(!_get_raw()){return 0;};
    joystick_x_cali = (double)buff[0];
    joystick_y_cali = (double)buff[1];
    calibrated = 1;

    return 1;
}

void destroy_nunchuk(){
    close_i2c_bus(nunchuk_file_i2c);
}

#define nunchuk_clamp(val) max( min(val, 1L), -1L)

int get_nunchuk( Nunchuk_Data *n ){
    if(!calibrated){
        if(!init_nunchuk()){return 0;}
    }
    else{
        if(!_get_raw()){return 0;}
    }

    n->joystick_x = nunchuk_clamp( ((double)buff[0] - joystick_x_cali)/96L );
    n->joystick_y = nunchuk_clamp( ((double)buff[1] - joystick_y_cali)/96L );

    n->c = (buff[5] & 0x02) == 0x00;
    n->z = (buff[5] & 0x01) == 0x00;

    n->acc_x = nunchuk_clamp(( (double)(( buff[2] << 2 ) | ((buff[5] >> 2) & 0x03)) - 512L ) / 512L);
    n->acc_y = nunchuk_clamp(( (double)(( buff[3] << 2 ) | ((buff[5] >> 4) & 0x03)) - 512L ) / 512L);
    n->acc_z = nunchuk_clamp(( (double)(( buff[4] << 2 ) | ((buff[5] >> 6) & 0x03)) - 512L ) / 512L);

    return 1;
}

int print_nunchuk(){
    Nunchuk_Data n;
    if(!get_nunchuk(&n)){return 0;};
    printf(
        "Nunchuk:\n"
        "    joystick x: %lf\n"
        "    joystick y: %lf\n"
        "    acceler x : %lf\n"
        "    acceler y : %lf\n"
        "    acceler z : %lf\n"
        "    c button  : %i\n"
        "    z button  : %i\n",
        n.joystick_x, n.joystick_y, n.acc_x, n.acc_y, n.acc_z, n.c, n.z);

    return 1;
}

int print_nunchuk_raw(){
    if(!calibrated){
        if(!init_nunchuk()){return 0;}
    }
    else{
        if(!_get_raw()){return 0;}
    }

    for(int i=0; i<NUNCHUK_PACKET_SIZE; i++){
        printf("Data[%i]: ", i);
        printBin(buff[i]);
    }
    return 1;
}
