#ifndef "gpio-led-demos/led_bar_graph.h"
#define "gpio-led-demos/led_bar_graph.h"

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

#endif
