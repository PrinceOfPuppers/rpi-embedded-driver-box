#include "nunchuk-demos/nunchuk-joystick-sensehat.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "driver-box-helpers.h"
#include "demo-helpers.h"
#include "sensehat.h"
#include "sigint-handler.h"
#include "gpio.h"
#include "nunchuk.h"
#include "led-bar-graph.h"


static int nunchuk_joystick_sensehat(int *stop_sig, char direction){
    Nunchuk_Data n;
    
    double x = 0.0;
    double y = 0.0;

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
        usleep(5000);
        if(!get_nunchuk(&n)){continue;};

        blit_pixel(_x, _y, 0.0, 0.0, 0.0);
        x = (n.joystick_x + 1.0) * (LED_MATRIX_WIDTH - 1) / 2;
        y = (n.joystick_y + 1.0) * (LED_MATRIX_WIDTH - 1) / 2;
        coordinate_converter(x, y, &_x, &_y);
        blit_pixel(_x, _y, r, g, b);
    }
    coordinate_converter(x, y, &_x, &_y);
    blit_pixel(_x, _y, 0.0, 0.0, 0.0);
    return 0;
}

typedef struct Nunchuk_Joystick_Sensehat_Thread_Data {
    int *stop_sig;
    char direction;
} Nunchuk_Joystick_Sensehat_Thread_Data;

static pthread_t tId;
static int _stop_sig = 0;
static int inited = 0;

static void *nunchuk_joystick_sensehat_thread(void *_d){
    Nunchuk_Joystick_Sensehat_Thread_Data *d= _d;

    int *stop_sig  = d->stop_sig;
    char direction = d->direction;
    nunchuk_joystick_sensehat(stop_sig, direction);
    free(d);
    return NULL;
}

void stop_nunchuk_joystick_sensehat(){
    if(inited && !_stop_sig){
        _stop_sig = 1;
        pthread_join(tId, NULL);
    }
}

int start_nunchuk_joystick_sensehat(char direction){
    stop_nunchuk_joystick_sensehat();
    _stop_sig = 0;

    Nunchuk_Joystick_Sensehat_Thread_Data *d = malloc(sizeof(Nunchuk_Joystick_Sensehat_Thread_Data));
    d->stop_sig = &_stop_sig;
    d->direction = direction;
    int error = pthread_create(&tId, NULL, nunchuk_joystick_sensehat_thread, (void*)d);
    if (error) {
        free(d);
        return 0;
    }
    inited = 1;

    return 1;
}
