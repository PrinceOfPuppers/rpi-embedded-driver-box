#include "nunchuk-demos/nunchuk-led-bar-graph.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>

#include "driver-box-helpers.h"
#include "sigint-handler.h"
#include "gpio.h"
#include "nunchuk.h"
#include "led-bar-graph.h"


int nunchuk_led_bar_graph(){
    int err = 0;
    Nunchuk_Data n;

    int prev_num = 0;
    int num;

    while(!sigint_triggered){
        if(!get_nunchuk(&n)){continue;};
        num = (int)((( (float)(n.joystick_y + 1) ) / 2.0) * (float)LED_BAR_GRAPH_GPIO_NUM  + 0.2);
        num = max(min(num, LED_BAR_GRAPH_GPIO_NUM -1), 0);

        if (led_bar_graph_set(prev_num, 0)) {
            err = 1; 
            goto nunchuk_led_bar_graph_test_end;
        }
        if (led_bar_graph_set(num, 1)) {
            err = 1; 
            goto nunchuk_led_bar_graph_test_end;
        }

        prev_num = num;
        usleep(NUNCHUK_MIN_POLLING_TIME_MICROSECONDS);
    }

    nunchuk_led_bar_graph_test_end:
    led_bar_graph_destroy();
    if(err){
        perror("");
        return 1;
    }
    return 0;
}

pthread_t tId;
int _stop_sig;
int inited;

void *nunchuk_led_bar_graph_thread(void *_){
    nunchuk_led_bar_graph(&_stop_sig);
    return NULL;
}

void stop_nunchuk_led_bar_graph(){
    if(inited && !_stop_sig){
        _stop_sig = 1;
        pthread_join(tId, NULL);
    }
}

int start_nunchuk_led_bar_graph(){
    stop_nunchuk_led_bar_graph();
    _stop_sig = 0;
    int error = pthread_create(&tId, NULL, nunchuk_led_bar_graph_thread, NULL);
    if (error) {
        return 0;
    }
    inited = 1;

    return 1;
}
