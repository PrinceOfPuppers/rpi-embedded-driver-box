#include "nunchuk-demos/nunchuk-sensehat.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include "driver-box-helpers.h"
#include "sensehat.h"
#include "sigint-handler.h"
#include "gpio.h"
#include "nunchuk.h"
#include "led-bar-graph.h"


static int nunchuk_sensehat(int *stop_sig){
    if(!init_sensehat_led_matrix()){
        return 0;
    }
    Nunchuk_Data n;
    
    double x = 0.0;
    double y = 0.0;

    double r = 1.0;
    double g = 1.0;
    double b = 1.0;

    while(!sigint_triggered && !*stop_sig){
        usleep(5000);
        if(!get_nunchuk(&n)){continue;};

        blit_pixel(x, y, 0.0, 0.0, 0.0);
        x = (n.joystick_x + 1.0) * (LED_MATRIX_WIDTH - 1) / 2;
        y = (n.joystick_y + 1.0) * (LED_MATRIX_WIDTH - 1) / 2;
        blit_pixel(x, y, r, g, b);
    }
    blit_pixel(x, y, 0.0, 0.0, 0.0);

    destroy_sensehat_led_matrix();
    return 0;
}

static pthread_t tId;
static int _stop_sig;
static int inited;

static void *nunchuk_sensehat_thread(void *_){
    nunchuk_sensehat(&_stop_sig);
    return NULL;
}

void stop_nunchuk_sensehat(){
    if(inited && !_stop_sig){
        _stop_sig = 1;
        pthread_join(tId, NULL);
    }
}

int start_nunchuk_sensehat(){
    stop_nunchuk_sensehat();
    _stop_sig = 0;
    int error = pthread_create(&tId, NULL, nunchuk_sensehat_thread, NULL);
    if (error) {
        return 0;
    }
    inited = 1;

    return 1;
}
