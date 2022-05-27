#include "gpio-led-demos/led-bar-graph.h"

#include <pthread.h>

#include "led_bar_graph.h"
#include "sigint-handler.h"

int led_bar_graph(int *stop_sig){
    int err = 0;

    int num;
    int prev_num = 0;
    int i = 0;
    while(!sigint_triggered && !*stop_sig){
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

pthread_t tId;
int _stop_sig;
int inited;

void *led_bar_graph_thread(void *){
    led_bar_graph(&_stop_sig);

    return NULL;
}

void stop_led_bar_graph(){
    if(inited && !_stopSig){
        _stopSig = 1;
        pthread_join(tId, NULL);
    }
}

int start_led_bar_graph(){
    stop_led_bar_graph();
    _stopSig = 0;
    int error = pthread_create(&tId, NULL, led_bar_graph_thread);
    if (error) {
        return 0;
    }
    inited = 1;

    return 1;
}
