#include "gpio-led-demos/gpio-led-demos.h"

#include <stdio.h>

#include "gpio-led-demos/led-bar-graph.h"
#include "gpio-led-demos/two-led-flash.h"

#include "sigint-handler.h"
#include "demo-helpers.h"

void gpio_led_cli(char *buffer, size_t *buffSize){
    printf("GPIO LED Interactive Mode:\n");

    char *cursor    ;
    char *cursorNext;

    char *arg1, *arg2, *arg3, *arg4;

    while(!sigint_triggered){
        printf("~ ");
        *buffSize = getline(&buffer, buffSize, stdin);
        cursor     = buffer;
        cursorNext = buffer;

        argGenerator(&cursor,&cursorNext);
        printf("%s: %i\n", cursor, smallHash(cursor));
        switch(smallHash(cursor)){
            case 1901:{ // graph
                if(!getNARgs(&cursor, &cursorNext, 2, &arg1, &arg2)){
                    //printf("2 Args, x And y, Are Required\n");
                    continue;
                break;
                };
            }

            case 1350224098:{ // rmgraph
                break;
            }


            case 1912:{ // flash
                break;
            }


            case 1350224065:{ // rmflash
                break;
            }


            case 113:{ //quit
                goto gpio_cli_clean_up;
            }


            case 1190:{ // help
                printf( "Commands:\n"
                        "   graph     cycles through led bar graph on gpio pins specified in led-bar-graph.h\n"
                        "   rmgraph   stops graph\n"
                        "   flash     flashes 2 leds on gpio pins specified in two-led-flash.h\n" 
                        "   rmflash   stops flash\n" 
                        "   q         quits\n");
                break;
            }
        }

    }
    
    gpio_cli_clean_up:
    stop_led_bar_graph();
    stop_two_led_flash();
    return;
}
