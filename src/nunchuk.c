#include "nunchuk.h"

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "driver-box-helpers.h"
#include "i2c.h"


static int inited = 0;
static int calibrated = 0;
static int nunchuk_file_i2c;

static int polling = 0;

#define NUNCHUK_PACKET_SIZE 6
static uint8_t out_buff[NUNCHUK_PACKET_SIZE] = {0};
static uint8_t in_buff[NUNCHUK_PACKET_SIZE] = {0};
static pthread_mutex_t out_buff_lock;
static pthread_mutex_t in_buff_lock;

static double joystick_x_cali;
static double joystick_y_cali;

static int _get_raw(){
    pthread_mutex_lock(&out_buff_lock);
    out_buff[0] = 0x00;
    if(!write_i2c(nunchuk_file_i2c, out_buff, 1)){
        pthread_mutex_unlock(&out_buff_lock);
        return 0;
    }
    pthread_mutex_unlock(&out_buff_lock);

    usleep(NUNCHUK_WRITE_READ_DELAY_MICROSECONDS);

    pthread_mutex_lock(&in_buff_lock);
    if(!read_i2c(nunchuk_file_i2c, in_buff, NUNCHUK_PACKET_SIZE)){
        pthread_mutex_unlock(&in_buff_lock);
        return 0;
    }
    pthread_mutex_unlock(&in_buff_lock);
    return 1;
}


int init_nunchuk(){
    if(!inited){
        if(pthread_mutex_init(&out_buff_lock, NULL) != 0){return 0;}
        if(pthread_mutex_init(&in_buff_lock, NULL) != 0){return 0;}
        
        char *filename = (char*)"/dev/i2c-1";
        int addr = 0x52;          //<<<<<The I2C address of the slave

        nunchuk_file_i2c = open_i2c_bus(filename, addr);
        if(nunchuk_file_i2c < 0){return 0;}

        pthread_mutex_lock(&out_buff_lock);
        out_buff[0] = 0xf0;
        out_buff[1] = 0x55;
        if(!write_i2c(nunchuk_file_i2c, out_buff, 2)){
            pthread_mutex_unlock(&out_buff_lock);
            return 0;
        }

        out_buff[0] = 0xfb;
        out_buff[1] = 0x00;
        if(!write_i2c(nunchuk_file_i2c, out_buff, 2)){
            pthread_mutex_unlock(&out_buff_lock);
            return 0;
        }
        pthread_mutex_unlock(&out_buff_lock);

        inited = 1;
        usleep(NUNCHUK_MIN_STARTUP_TIME_MICROSECONDS);
    }

    if(!_get_raw()){return 0;};
    pthread_mutex_lock(&in_buff_lock);
    joystick_x_cali = (double)in_buff[0];
    joystick_y_cali = (double)in_buff[1];
    pthread_mutex_unlock(&in_buff_lock);
    calibrated = 1;

    return 1;
}

static pthread_t polling_tId;

static void *polling_thread(void *_){
    while(polling){
        _get_raw();
        usleep(NUNCHUK_POLLING_TIME_MICROSECONDS);
    }
    return NULL;
}

int start_nunchuk_polling(){
    if(polling){
        return 1;
    }
    polling = 1;
    if(!calibrated){
        if(!init_nunchuk()){return 0;}
    }

    int error = pthread_create(&polling_tId, NULL, polling_thread, NULL);
    if (error) {
        polling = 0;
        return 0;
    }
    return 1;
}

void stop_nunchuk_polling(){
    if(!polling){
        return;
    }
    polling = 0;
    pthread_join(polling_tId, NULL);
}

void destroy_nunchuk(){
    stop_nunchuk_polling();
    pthread_mutex_destroy(&out_buff_lock);
    pthread_mutex_destroy(&in_buff_lock);
    close_i2c_bus(nunchuk_file_i2c);
    inited = 0;
    calibrated = 0;
}

#define nunchuk_clamp(val) max( min(val, 1L), -1L)

static void read_data(Nunchuk_Data *n){
    pthread_mutex_lock(&in_buff_lock);
    n->joystick_x = nunchuk_clamp( ((double)in_buff[0] - joystick_x_cali)/96L );
    n->joystick_y = nunchuk_clamp( ((double)in_buff[1] - joystick_y_cali)/96L );

    n->c = (in_buff[5] & 0x02) == 0x00;
    n->z = (in_buff[5] & 0x01) == 0x00;

    n->acc_x = nunchuk_clamp(( (double)(( in_buff[2] << 2 ) | ((in_buff[5] >> 2) & 0x03)) - 512L ) / 512L);
    n->acc_y = nunchuk_clamp(( (double)(( in_buff[3] << 2 ) | ((in_buff[5] >> 4) & 0x03)) - 512L ) / 512L);
    n->acc_z = nunchuk_clamp(( (double)(( in_buff[4] << 2 ) | ((in_buff[5] >> 6) & 0x03)) - 512L ) / 512L);
    pthread_mutex_unlock(&in_buff_lock);
}

int get_nunchuk( Nunchuk_Data *n ){
    if(!calibrated){
        if(!init_nunchuk()){return 0;}
    }
    else{
        if(!polling){
            if(!_get_raw()){return 0;}
        }
    }

    read_data(n);
    return 1;
}

int print_nunchuk_raw(){
    if(!calibrated){
        if(!init_nunchuk()){return 0;}
    }
    else{
        if(!polling){
            if(!_get_raw()){return 0;}
        }
    }

    pthread_mutex_lock(&in_buff_lock);
    for(int i=0; i<NUNCHUK_PACKET_SIZE; i++){
        printf("Data[%i]: ", i);
        printBin(in_buff[i]);
    }
    pthread_mutex_unlock(&in_buff_lock);
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

