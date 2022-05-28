#include "cli.h"

#include <stdlib.h>

#include "demo-helpers.h"
#include "sigint-handler.h"
#include "sensehat-demos/sensehat-demos.h"
#include "gpio-led-demos/gpio-led-demos.h"

void cli(){
    size_t buffSize = 64;
    char *buffer = malloc(buffSize*sizeof(char));

    //sensehat_cli(buffer, buffSize);
    gpio_led_cli(buffer, &buffSize);

    free(buffer); 
}
