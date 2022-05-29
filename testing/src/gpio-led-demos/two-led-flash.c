#include "gpio-led-demos/two-led-flash.h"

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "sigint-handler.h"
#include "gpio.h"

int two_led_flash(int *stop_sig){
    int fd1 = export_pin(LED_FLASH_GPIO_PIN_1, "out");
    int fd2 = export_pin(LED_FLASH_GPIO_PIN_2, "out");
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

    if(!unexport_pin(LED_FLASH_GPIO_PIN_1, fd1) || !unexport_pin(LED_FLASH_GPIO_PIN_2, fd2)){return 1;}
    return 0;
}


pthread_t tId;
int _stop_sig;
int inited;

void *two_led_flash_thread(void *_){
    two_led_flash(&_stop_sig);

    return NULL;
}

void stop_two_led_flash(){
    if(inited && !_stop_sig){
        _stop_sig = 1;
        pthread_join(tId, NULL);
    }
}

int start_two_led_flash(){
    stop_two_led_flash();
    _stop_sig = 0;
    int error = pthread_create(&tId, NULL, two_led_flash_thread, NULL);
    if (error) {
        return 0;
    }
    inited = 1;

    return 1;
}
