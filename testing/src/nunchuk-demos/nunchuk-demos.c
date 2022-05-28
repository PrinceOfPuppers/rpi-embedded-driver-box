#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "driver-box-helpers.h"
#include "sigint-handler.h"
#include "gpio.h"
#include "nunchuk.h"
#include "led-bar-graph.h"


int nunchuk_led_bar_graph(){
    int err = 0;
    Nunchuk n;

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
        usleep(100000);
    }

    nunchuk_led_bar_graph_test_end:
    led_bar_graph_destroy();
    if(err){
        perror("");
        return 1;
    }
    return 0;
}

int nunchuk_test()
{
    Nunchuk n;

    int i=0;
    while(!sigint_triggered){
        printf("loop: %i\n", i);
        if(!get_nunchuk(&n)){continue;};
        printf("(%lf, %lf)\n", n.joystick_x, n.joystick_y);
        printf("(%s, %s)\n", n.c ? "c" : " ", n.z ? "z" : " ");

        i++;
        usleep( MIN_POLLING_TIME_MICROSECONDS );
    }
    return 0;
}
