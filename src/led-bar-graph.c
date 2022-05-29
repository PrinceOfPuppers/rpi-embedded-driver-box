#include "led-bar-graph.h"

#include <stdio.h>
#include <unistd.h>

#include "driver-box-helpers.h"
#include "gpio.h"

static int fds[LED_BAR_GRAPH_GPIO_NUM];
static int initted = 0;

int led_bar_graph_init(){
    if(initted){
        led_bar_graph_destroy();
    }

    if (!export_pins(LED_BAR_GRAPH_GPIO_START, LED_BAR_GRAPH_GPIO_NUM, fds, "out")){
        return 0;
    }

    initted = 1;
    return 1;
}

void led_bar_graph_destroy(){
    // no need to check initted
    for(int i = 0; i < LED_BAR_GRAPH_GPIO_NUM; i++){
        if(fds[i] < 0){
            continue;
        }
        write(fds[i], "0", 1);
    }

    unexport_pins(LED_BAR_GRAPH_GPIO_START, LED_BAR_GRAPH_GPIO_NUM, fds);

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
