#include "gpio-led-demos/two_led_flash.h"

#include <pthread.h>

#include "sigint-handler.h"
#include "gpio.h"

#define GPIO_PIN_1 "14"
#define GPIO_PIN_2 "15"


int two_led_flash(int *stop_sig){
    int fd1 = export_pin(GPIO_PIN_1, "out");
    int fd2 = export_pin(GPIO_PIN_2, "out");
    if (fd1 < 0 || fd2 < 0){return 1;}

    while(!sigint_triggered && !*stop_sig) {
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


pthread_t tId;
int _stop_sig;
int inited;

void *twoLedFlashThread(void *){
    two_led_flash(&_stop_sig);

    return NULL;
}

void stopTwoLedFlash(){
    if(inited && !_stopSig){
        _stopSig = 1;
        pthread_join(tId, NULL);
    }
}

int startTwoLedFlash(){
    stopTwoLedFlash();
    _stopSig = 0;
    int error = pthread_create(&tId, NULL, twoLedFlashThread);
    if (error) {
        return 0;
    }
    inited = 1;

    return 1;
}
