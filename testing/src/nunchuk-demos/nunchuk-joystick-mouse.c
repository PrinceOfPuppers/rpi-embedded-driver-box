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


void nunchuk_fill_mouse_report(char report[MOUSE_REPORT_SIZE], Nunchuk_Data *n){
    double mouse_x = fabs(n->joystick_x) < MOUSE_JOYSTICK_DEADZONE_X ? 0.0 : n->joystick_x*MAX_MOUSE_MOVE_DOUBLE;
    double mouse_y = fabs(n->joystick_y) < MOUSE_JOYSTICK_DEADZONE_Y ? 0.0 : -n->joystick_y*MAX_MOUSE_MOVE_DOUBLE;

    report[0] = (n->c)*LMB_VAL | (n->z)*RMB_VAL;
    report[1] = (int)(MOUSE_JOYSTICK_SENSITIVITY_X*mouse_x);
    report[2] = (int)(MOUSE_JOYSTICK_SENSITIVITY_Y*mouse_y);
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
