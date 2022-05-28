#include "gpio-led-demos/gpio-led-demos.h"

#include <stdio.h>

#include "gpio-led-demos/led-bar-graph.h"
#include "gpio-led-demos/two-led-flash.h"

#include "sigint-handler.h"
#include "demo-helpers.h"

void gpio_led_cli(char *buffer, size_t *buffSize){
    char *cursor    ;
    char *cursorNext;

    while(!sigint_triggered){
        printf("\nGPIO LED Demos (type help for options):\n");
        printf("~ ");
        *buffSize = getline(&buffer, buffSize, stdin);
        if(sigint_triggered){break;}
        cursor     = buffer;
        cursorNext = buffer;

        argGenerator(&cursor,&cursorNext);
        switch(smallHash(cursor)){

            case 634:{ // bar
                start_led_bar_graph();
                break;
            }

            case 1909:{ // rmbar
                stop_led_bar_graph();
                break;
            }


            case 1912:{ // flash
                start_two_led_flash();
                break;
            }


            case 4474:{ // rmflash
                stop_two_led_flash();
                break;
            }


            case 113:{ // q
                goto gpio_cli_clean_up;
            }


            case 1190:{ // help
                printf( "Commands:\n"
                        "   bar       cycles through led bar graph on gpio pins specified in led-bar-graph.h\n"
                        "   rmbar     stops bar\n"
                        "   flash     flashes 2 leds on gpio pins specified in two-led-flash.h\n" 
                        "   rmflash   stops flash\n" 
                        "   q         quits to previous menu\n");
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
    
    gpio_cli_clean_up:
    stop_led_bar_graph();
    stop_two_led_flash();
    return;
}
