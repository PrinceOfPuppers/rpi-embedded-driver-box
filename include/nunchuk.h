#ifndef NUNCHUK_H_
#define NUNCHUK_H_

#define NUNCHUK_DEBUG 0


#define NUNCHUK_WRITE_READ_DELAY_MICROSECONDS 200 // time between request to get data and reading data
#define NUNCHUK_POLLING_TIME_MICROSECONDS 5000    // total polling time will be this plus NUNCHUK_WRITE_READ_DELAY_MICROSECONDS 

// minimum stable values
#define MIN_NUNCHUK_WRITE_READ_DELAY_MICROSECONDS 200
#define MIN_NUNCHUK_POLLING_TIME_MICROSECONDS 1000


#define NUNCHUK_MIN_STARTUP_TIME_MICROSECONDS 1000

struct Nunchuk_Data{
    // all from -1 to 1
    double joystick_x;
    double joystick_y;

    double acc_x;
    double acc_y;
    double acc_z;

    // acc but in units of g
    double acc_x_g;
    double acc_y_g;
    double acc_z_g;

    int c; // pressed == true
    int z; // pressed == true
};
typedef struct Nunchuk_Data Nunchuk_Data;

int init_nunchuk();

int start_nunchuk_polling();

void stop_nunchuk_polling();

void destroy_nunchuk();

int get_nunchuk(Nunchuk_Data* n);

int print_nunchuk();

int print_nunchuk_raw();

#endif
