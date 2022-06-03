#include "nunchuk-demos/nunchuk-joystick-mouse.h"

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "sigint-handler.h"
#include "nunchuk.h"

#define LMB_VAL 0x01
#define RMB_VAL 0x02
#define MMB_VAL 0x04

#define MAX_MOUSE_MOVE_DOUBLE 127.0
#define MOUSE_REPORT_SIZE 3



static int _shake_end_countdown = 0;
static int _shake_start_countdown = SHAKE_STARTUP_TIMER;
static int _shaking = 0;

// off: -1, lmb: 0, rmb: 1, mmb: 2
static int _shake_click = 1;
static int _clicks[] = {LMB_VAL, RMB_VAL, MMB_VAL};

int is_shaking(Nunchuk_Data *n){
    double g_force = sqrt(n->acc_x_g*n->acc_x_g + n->acc_y_g*n->acc_y_g + n->acc_z_g*n->acc_z_g);
    int shake = g_force > MAX_NOSHAKE_RANGE || g_force < MIN_NOSHAKE_RANGE ? 1 : 0; 

    if(shake){

        // shake startup 
        if(!_shaking){
            printf("shake startup\n");
            if(_shake_start_countdown < 1){
                _shake_start_countdown = SHAKE_STARTUP_TIMER;
                _shaking = 1;
            }
            else{
                _shake_start_countdown--;
                return 0;
            }
        }

        
        _shake_end_countdown = SHAKE_PERSIST_TIMER;
        printf("g_force: %f\n", g_force);
        return 1;
    }

    if(_shake_end_countdown == 1){
        printf("shake over\n");
    }
    _shake_end_countdown--;
    if(_shake_end_countdown < 1){
        _shake_end_countdown = 0;

        _shaking = 0;
        return 0;
    }
    printf("shake ending\n");
    printf("g_force: %f\n", g_force);
    return 1;
}

void nunchuk_fill_mouse_report(char report[MOUSE_REPORT_SIZE], Nunchuk_Data *n){
    double mouse_x = fabs(n->joystick_x) < MOUSE_JOYSTICK_DEADZONE_X ? 0.0 : n->joystick_x*MAX_MOUSE_MOVE_DOUBLE;
    double mouse_y = fabs(n->joystick_y) < MOUSE_JOYSTICK_DEADZONE_Y ? 0.0 : -n->joystick_y*MAX_MOUSE_MOVE_DOUBLE;

    report[0] = (n->c)*LMB_VAL | (n->z)*RMB_VAL;
    report[1] = (int)(MOUSE_JOYSTICK_SENSITIVITY_X*mouse_x);
    report[2] = (int)(MOUSE_JOYSTICK_SENSITIVITY_Y*mouse_y);

    if(_shake_click > -1){
        if(is_shaking(n)){
            report[0] |= _clicks[_shake_click];
            printf("Shake !!\n");
        }

    }
}

#define MOUSE_RECONNECT_INTERVAL_MICROSECONDS 5000000
int await_mouse_connect(int *stop_sig, char *device_file){
    while(!sigint_triggered && !*stop_sig){
        int fd = open(device_file, O_RDWR, 0666);
        if (fd >= 0) {
          return fd;
        }
        perror(device_file);
        usleep(MOUSE_RECONNECT_INTERVAL_MICROSECONDS);
    }
    return -1;
}


void nunchuk_joystick_mouse(int *stop_sig, char *device_file){
    char report[MOUSE_REPORT_SIZE];
    Nunchuk_Data n;

    int fd = await_mouse_connect(stop_sig, device_file);
    if(fd < 0){return;}

    while(!sigint_triggered && !*stop_sig){
        get_nunchuk(&n);
        nunchuk_fill_mouse_report(report, &n);
        if (write(fd, report, MOUSE_REPORT_SIZE) != MOUSE_REPORT_SIZE) {
            perror(device_file);
            close(fd);
            fd = await_mouse_connect(stop_sig, device_file);
            if(fd < 0){return;}
        }
        usleep(MOUSE_POLLING_INTERVAL_MICROSECONDS);
    }

    close(fd);
    return;
}


typedef struct Nunchuk_Joystick_Mouse_Thread_Data {
    int *stop_sig;
    char *device_file;
} Nunchuk_Joystick_Mouse_Thread_Data;

static pthread_t tId;
static int _stop_sig = 0;
static int inited = 0;

static void *nunchuk_joystick_mouse_thread(void *_d){
    Nunchuk_Joystick_Mouse_Thread_Data *d= _d;

    int *stop_sig  = d->stop_sig;
    char *device_file = d->device_file;
    nunchuk_joystick_mouse(stop_sig, device_file);
    free(d);
    return NULL;
}

void stop_nunchuk_joystick_mouse(){
    if(inited && !_stop_sig){
        _stop_sig = 1;
        pthread_join(tId, NULL);
    }
}

int start_nunchuk_joystick_mouse(char *device_file){
    stop_nunchuk_joystick_mouse();
    _stop_sig = 0;

    Nunchuk_Joystick_Mouse_Thread_Data *d = malloc(sizeof(Nunchuk_Joystick_Mouse_Thread_Data));
    d->stop_sig = &_stop_sig;
    d->device_file = device_file;
    int error = pthread_create(&tId, NULL, nunchuk_joystick_mouse_thread, (void*)d);
    if (error) {
        free(d);
        return 0;
    }
    inited = 1;

    return 1;
}
int nunchuk_joystick_mouse_started(){
    return inited;
}

void start_nunchuk_shake_click(char button){
    switch(button){
        case 'l':{
            _shake_click = 0;
            break;
        }
        case 'r':{
            _shake_click = 1;
            break;
        }
        case 'm':{
            _shake_click = 2;
            break;
        }
        default:{
            _shake_click = -1;
            break;

        }
    }
}

void stop_nunchuk_shake_click(){
    _shake_click = -1;
}
