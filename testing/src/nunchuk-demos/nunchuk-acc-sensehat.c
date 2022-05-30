#include "nunchuk-demos/nunchuk-acc-sensehat.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include "driver-box-helpers.h"
#include "demo-helpers.h"
#include "sensehat.h"
#include "sigint-handler.h"
#include "gpio.h"
#include "nunchuk.h"
#include "led-bar-graph.h"

#define NUNCHUK_SENSEHAT_ACC_CONSTANT 0.000005
#define NUNCHUK_SENSEHAT_COLLISION_DAMPING 0.6
#define NUNCHUK_SENSEHAT_TIMESTEP 50000


static int nunchuk_acc_sensehat(int *stop_sig, char direction){
    Nunchuk_Data n;
    
    double x = ((double)LED_MATRIX_WIDTH - 1)/ 2;
    double y = ((double)LED_MATRIX_WIDTH - 1)/ 2;

    double vx = 0.0;
    double vy = 0.0;

    double r = 1.0;
    double g = 1.0;
    double b = 1.0;

    void (*coordinate_converter)();

    switch(direction){
        case 'r': coordinate_converter = port_right_convert; break;
        case 'l': coordinate_converter = port_left_convert; break;
        case 'u': coordinate_converter = port_up_convert; break;
        default: coordinate_converter = port_down_convert; break;
    }
    double _x;
    double _y;
    coordinate_converter(x, y, &_x, &_y);

    while(!sigint_triggered && !*stop_sig){
        usleep(NUNCHUK_SENSEHAT_TIMESTEP);
        if(!get_nunchuk(&n)){continue;};

        blit_pixel(_x, _y, 0.0, 0.0, 0.0);
        vx += n.acc_x * NUNCHUK_SENSEHAT_ACC_CONSTANT * NUNCHUK_SENSEHAT_TIMESTEP;
        vy += n.acc_y * NUNCHUK_SENSEHAT_ACC_CONSTANT * NUNCHUK_SENSEHAT_TIMESTEP;

        x += vx;
        y += vy;

        if(x < 0.0){
            x = 0.0;
            vx = -NUNCHUK_SENSEHAT_COLLISION_DAMPING*vx;
        }
        if(x > LED_MATRIX_WIDTH - 1){
            x = LED_MATRIX_WIDTH - 1;
            vx = -NUNCHUK_SENSEHAT_COLLISION_DAMPING*vx;
        }

        if(y < 0.0){
            y = 0.0;
            vy = -NUNCHUK_SENSEHAT_COLLISION_DAMPING*vy;
        }
        if(y > LED_MATRIX_WIDTH - 1){
            y = LED_MATRIX_WIDTH - 1;
            vy = -NUNCHUK_SENSEHAT_COLLISION_DAMPING*vy;
        }

        coordinate_converter(x, y, &_x, &_y);
        blit_pixel(_x, _y, r, g, b);
    }
    coordinate_converter(x, y, &_x, &_y);
    blit_pixel(_x, _y, 0.0, 0.0, 0.0);
    return 0;
}

typedef struct Nunchuk_Acc_Sensehat_Thread_Data {
    int *stop_sig;
    char direction;
} Nunchuk_Acc_Sensehat_Thread_Data;

static pthread_t tId;
static int _stop_sig = 0;
static int inited = 0;

static void *nunchuk_acc_sensehat_thread(void *_d){
    Nunchuk_Acc_Sensehat_Thread_Data *d= _d;

    int *stop_sig  = d->stop_sig;
    char direction = d->direction;

    nunchuk_acc_sensehat(stop_sig, direction);
    return NULL;
}

void stop_nunchuk_acc_sensehat(){
    if(inited && !_stop_sig){
        _stop_sig = 1;
        pthread_join(tId, NULL);
    }
}

int start_nunchuk_acc_sensehat(char direction){
    stop_nunchuk_acc_sensehat();
    _stop_sig = 0;
    Nunchuk_Acc_Sensehat_Thread_Data *d = malloc(sizeof(Nunchuk_Acc_Sensehat_Thread_Data));
    d->stop_sig = &_stop_sig;
    d->direction = direction;
    int error = pthread_create(&tId, NULL, nunchuk_acc_sensehat_thread, (void*)d);
    if (error) {
        free(d);
        return 0;
    }
    inited = 1;

    return 1;
}
