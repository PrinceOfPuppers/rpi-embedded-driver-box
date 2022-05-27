#include "gpio-led-demos/gpio-led-demos.h"

#include "gpio-led-demos/led-bar-graph.h"
#include "gpio-led-demos/two-led-flash.h"

void gpio_led_cli(char *buff){
    printf("GPIO LED Interactive Mode:\n");

    char * cursor    ;
    char * cursorNext;

    char *arg1, *arg2, *arg3, *arg4;

    while(!sigint_triggered){
        printf("~ ");
        buffSize = getline(&buffer,&buffSize,stdin);
        cursor     = buffer;
        cursorNext = buffer;

        argGenerator(&cursor,&cursorNext);
        printf("%s: %i\n", cursor, smallHash(cursor));
        switch(smallHash(cursor)){

            case 665:{ // set
                if(!getNARgs(&cursor, &cursorNext, 2, &arg1, &arg2)){
                    if(fixed){
                        printf("2 Args, x And y, Are Required\n");
                    }else{
                        printf("5 Args, x, y, r, g, b, Are Required\n");
                    }
                    continue;
                };
                int x = atoi(arg1);
                int y = atoi(arg2);

                if(fixed){
                    printf("Setting x: %i y: %i to fixed (r,g,b): (%f, %f, %f)\n",x,y,fixr,fixg,fixb);
                    setVal(map,x,y,rgbDoubleToHex(fixr,fixg,fixb));
                    continue;
                }

                if(!getNARgs(&cursor, &cursorNext, 3, &arg1, &arg2, &arg3)){
                    printf("5 Args, x, y, r, g, b, Are Required\n");
                    continue;
                };
                double r = (double)atof(arg1);
                double g = (double)atof(arg2);
                double b = (double)atof(arg3);

                printf("Setting x: %i y: %i to (r,g,b): (%f, %f, %f)\n",x,y,r,g,b);
                setVal(map,x,y,rgbDoubleToHex(r,g,b));
                break;
            }

            case 113:{ //quit
                break;
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

    stop_led_bar_graph():
    stop_two_led_flash():
    return;
}
