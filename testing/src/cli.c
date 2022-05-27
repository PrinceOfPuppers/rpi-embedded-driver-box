#include "cli.h"

#include <stdlib.h>

#include "demo-helpers.h"
#include "sigint-handler.h"
#include "sensehat-demos/sensehat-demos.h"
#include "gpio-led-demos/gpio-led-demos.h"

void cli(){
    size_t buff_size = 10;
    char *buffer = malloc(buff_size*sizeof(char));

    //sensehat_cli(buffer);
    gpio_led_cli(buffer);

    free(buffer); 
}
