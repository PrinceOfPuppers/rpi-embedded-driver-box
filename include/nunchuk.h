#ifndef NUNCHUK_H_
#define NUNCHUK_H_

#define NUNCHUK_DEBUG 0

#define MIN_POLLING_TIME_MICROSECONDS 50000
#define MIN_STARTUP_TIME_MICROSECONDS 1000000

struct Nunchuk{
    // all from -1 to 1
    double joystick_x;
    double joystick_y;

    //double acc_x;
    //double acc_y;
    //double acc_z;

    int c; // pressed == true
    int z; // pressed == true
};
typedef struct Nunchuk Nunchuk;

int get_nunchuk(Nunchuk* n);

#endif
