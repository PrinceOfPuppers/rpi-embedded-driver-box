#include "nunchuk.h"
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define LMB_VAL 0x01
#define RMB_VAL 0x02
#define MMB_VAL 0x04

#define MAX_MOUSE_MOVE_DOUBLE 127.0
#define MOUSE_REPORT_SIZE 3

#define MOUSE_JOYSTICK_DEADZONE_X 0.06
#define MOUSE_JOYSTICK_DEADZONE_Y 0.03

#define MOUSE_JOYSTICK_SENSITIVITY_X 1.0
#define MOUSE_JOYSTICK_SENSITIVITY_Y 1.0

#define MOUSE_POLLING_INTERVAL_MICROSECONDS 8333 // approx 120 HZ

void nunchuk_fill_mouse_report(char report[MOUSE_REPORT_SIZE], Nunchuk_Data *n){
    double mouse_x = fabs(n->joystick_x) < MOUSE_JOYSTICK_DEADZONE_X ? 0.0 : n->joystick_x*MAX_MOUSE_MOVE_DOUBLE;
    double mouse_y = fabs(n->joystick_y) < MOUSE_JOYSTICK_DEADZONE_Y ? 0.0 : -n->joystick_y*MAX_MOUSE_MOVE_DOUBLE;

    report[0] = (n->c)*LMB_VAL | (n->z)*RMB_VAL;
    report[1] = (int)(MOUSE_JOYSTICK_SENSITIVITY_X*mouse_x);
    report[2] = (int)(MOUSE_JOYSTICK_SENSITIVITY_Y*mouse_y);
}


int get_mouse_fd(){
    const char *filename = "/dev/hidg0";
    return open(filename, O_RDWR, 0666);
}


int nunchuk_mouse(){
    const char *filename = "/dev/hidg0";
    start_nunchuk_polling();

    char report[MOUSE_REPORT_SIZE];
    Nunchuk_Data n;

    int fd = get_mouse_fd();

    if (fd < 0) {
      perror(filename);
      return 1;
    }

    while(1) {
        get_nunchuk(&n);
        nunchuk_fill_mouse_report(report, &n);
        if (write(fd, report, MOUSE_REPORT_SIZE) != MOUSE_REPORT_SIZE) {
            perror(filename);
            return 1;
        }
        usleep(MOUSE_POLLING_INTERVAL_MICROSECONDS);
    }

    close(fd);
    destroy_nunchuk();
    return 0;
}
