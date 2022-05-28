#include "cli.h"

#include <stdlib.h>
#include <stdio.h>

#include "demo-helpers.h"
#include "sigint-handler.h"
#include "sensehat-demos/sensehat-demos.h"
#include "nunchuk-demos/nunchuk-demos.h"
#include "gpio-led-demos/gpio-led-demos.h"

void cli(){
    size_t buffSize = 64;
    char *buffer = malloc(buffSize*sizeof(char));

    char *help_message = \
            "demo packs:\n"
            "   sensehat    raspberry pi sense hat\n"
            "   leds        gpio blinking leds \n"
            "   nunchuk     wii nunchuk demos (may also involve other hardware)\n" 
            "   q           quits\n";

    char *cursor    ;
    char *cursorNext;

    while(!sigint_triggered){
        printf("\nInteractive Demos (type help to see options):\n");
        printf("~ ");
        buffSize = getline(&buffer, &buffSize, stdin);
        if(sigint_triggered){break;}
        cursor     = buffer;
        cursorNext = buffer;

        argGenerator(&cursor,&cursorNext);
        switch(smallHash(cursor)){

            case 1185:{ // leds
                gpio_led_cli(buffer, &buffSize);
                break;
            }

            case 6306:{ // sensehat
                sensehat_cli(buffer, &buffSize);
                break;
            }


            case 4575:{ // nunchuk
                nunchuk_cli(buffer, &buffSize);
                break;
            }

            case 113:{ // q
                goto toplevel_cli_cleanup;
            }

            case 1190:{ // help
                printf(help_message);
                break;
            }

            case 0:{
                break;
            }

            default:{
                printf("%s is not an option\n", cursor);
            }
        }
    }
    
    toplevel_cli_cleanup:
    free(buffer); 
    return;
}
