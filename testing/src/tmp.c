#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "driver-box-helpers.h"
#include "sigint-handler.h"
#include "gpio.h"
#include "nunchuk.h"

#define GPIO_PIN_1 "14"
#define GPIO_PIN_2 "15"

int gpioTest1(){
    int fd1 = export_pin(GPIO_PIN_1, "out");
    int fd2 = export_pin(GPIO_PIN_2, "out");
    if (fd1 < 0 || fd2 < 0){return 1;}

    while(!sigint_triggered) {
        if (write(fd1, "1", 1) != 1) {
            return 1;
        }
        if (write(fd2, "0", 1) != 1) {
            return 1;
        }
        usleep(500000);

        if (write(fd1, "0", 1) != 1) {
            return 1;
        }

        if (write(fd2, "1", 1) != 1) {
            return 1;
        }
        usleep(500000);
    }

    if (write(fd1, "0", 1) != 1) {
        return 1;
    }
    if (write(fd2, "0", 1) != 1) {
        return 1;
    }

    if(!unexport_pin("14", fd1) || !unexport_pin("15", fd2)){return 1;}
    return 0;
}

int led_bar_graph_test(){
    int err = 0;

    int num;
    int prev_num = 0;
    int i = 0;
    while(!sigint_triggered){
        num = i % GPIO_NUM;
        if (led_bar_graph_set(prev_num, 0)) {
            err = 1; 
            goto led_bar_graph_test_end;
        }
        if (led_bar_graph_set(num, 1)) {
            err = 1; 
            goto led_bar_graph_test_end;
        }
        prev_num = num;
        i++;
        usleep(500000);
    }

    led_bar_graph_test_end:
    led_gar_graph_destroy();
    if(err){
        perror("");
        return 1;
    }
    return 0;
}


int nunchuk_led_bar_graph_test(){
    int err = 0;
    Nunchuk n;

    int prev_num = 0;
    int num;

    while(!sigint_triggered){
        if(!get_nunchuk(&n)){continue;};
        num = (int)((( (float)(n.joystick_y + 1) ) / 2.0) * (float)GPIO_NUM + 0.2);
        num = max(min(num, GPIO_NUM-1), 0);

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
    led_gar_graph_destroy(fds, pin_num_buff);
    if(err){
        perror("");
        return 1;
    }
    return 0;
}

int nunchuk_test()
{
    Nunchuk n;

    i=0;
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
