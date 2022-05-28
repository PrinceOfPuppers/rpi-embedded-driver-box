#include "led-bar-graph.h"

#include <stdio.h>
#include <unistd.h>

#include "driver-box-helpers.h"
#include "gpio.h"

int fds[LED_BAR_GRAPH_GPIO_NUM];
char pin_num_buff[10];
int initted = 0;

int led_bar_graph_init(){
    if(initted){
        led_bar_graph_destroy();
    }

    for(int i = 0; i < LED_BAR_GRAPH_GPIO_NUM; i++){
        fds[i] = -1;
    }

    int pin_num;

    for(int i = 0; i < LED_BAR_GRAPH_GPIO_NUM; i++){
        pin_num = i + LED_BAR_GRAPH_GPIO_START;
        sprintf(pin_num_buff ,"%i", pin_num);
        fds[i] = export_pin(pin_num_buff, "out");
        write(fds[i], "0", 1);
        if(fds[i]<0){
            return 0;
        }
    }

    initted = 1;
    return 1;
}

void led_bar_graph_destroy(){
    // no need to check initted
    int pin_num;
    for(int i = 0; i < LED_BAR_GRAPH_GPIO_NUM; i++){
        if(fds[i] < 0){
            continue;
        }
        pin_num = i + LED_BAR_GRAPH_GPIO_START;
        write(fds[i], "0", 1);
        sprintf(pin_num_buff ,"%i", pin_num);
        unexport_pin(pin_num_buff, fds[i]);
    }

    initted = 0;
}


int led_bar_graph_set(int i, int zero_or_one){
    i = max(min(i, LED_BAR_GRAPH_GPIO_NUM-1), 0);
    if(!initted){
        if(!led_bar_graph_init()){
            return 0;
        }
    }

    switch(zero_or_one){
        case 0:
            if (write(fds[i], "0", 1) != 1) {return 0;}
            break;
        default:
            if (write(fds[i], "1", 1) != 1) {return 0;}
            break;
    }
    return 1;
}
