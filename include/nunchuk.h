#ifndef NUNCHUK_H_
#define NUNCHUK_H_

#define NUNCHUK_DEBUG 0

#define NUNCHUK_MIN_POLLING_TIME_MICROSECONDS 1000
#define NUNCHUK_MIN_STARTUP_TIME_MICROSECONDS 1000

struct Nunchuk_Data{
    // all from -1 to 1
    double joystick_x;
    double joystick_y;

    double acc_x;
    double acc_y;
    double acc_z;

    int c; // pressed == true
    int z; // pressed == true
};
typedef struct Nunchuk_Data Nunchuk_Data;

int init_nunchuk();

void destroy_nunchuk();

int get_nunchuk(Nunchuk_Data* n);

int print_nunchuk();

int print_nunchuk_raw();

#endif
