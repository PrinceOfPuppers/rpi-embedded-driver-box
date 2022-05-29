#include "sensehat-demos/sensehat-demos.h"

#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "sensehat.h"
#include "demo-helpers.h"

#include "sensehat-demos/ball.h"
#include "sensehat-demos/digital-rain.h"
#include "sigint-handler.h"



void sensehat_cli(char *buffer, size_t *buffSize){
    if(!init_sensehat_led_matrix()){
        return;
    }

    time_t t;
    srand((unsigned) time(&t));

    Balls balls;
    balls._nextBall = 0;


    char * cursor    ;
    char * cursorNext;

    int fixed = 0;

    double fixr = 0.0;
    double fixg = 0.0;
    double fixb = 0.0;


    char *arg1, *arg2, *arg3, *arg4;

    while(!sigint_triggered){
        printf("\nSenseHat Demos (type help for options):\n");
        printf("~ ");
        *buffSize = getline(&buffer, buffSize, stdin);
        if(sigint_triggered){break;}
        cursor     = buffer;
        cursorNext = buffer;

        argGenerator(&cursor,&cursorNext);
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
                    led_matrix_set_val(x,y,rgb_double_to_hex(fixr,fixg,fixb));
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
                led_matrix_set_val(x,y,rgb_double_to_hex(r,g,b));
                break;
            }


            case 1153:{ //line
                if(!getNARgs(&cursor, &cursorNext, 4, &arg1, &arg2, &arg3, &arg4)){
                    if(fixed){
                        printf("4 Args, startX, startY, endX, endY, Are Required\n");
                    }
                    else{
                        printf("7 Args, startX, startY, endX, endY, r, g, b, Are Required\n");
                    }
                    continue;
                };
                int x = atoi(arg1);
                int y = atoi(arg2);
                int endX = atoi(arg3);
                int endY = atoi(arg4);

                if(fixed){
                    printf("Setting Pixels on Line from (%i,%i) to (%i, %i) (r,g,b): (%f, %f, %f)\n",x,y,endX,endY,fixr,fixg,fixb);
                    blit_colored_line(x, y, endX, endY, fixr, fixg, fixb);
                    continue;
                }
                if(!getNARgs(&cursor, &cursorNext, 3, &arg1, &arg2, &arg3)){
                    printf("7 Args, startX, startY, endX, endY, r, g, b, Are Required\n");
                    continue;
                };
                double r = (double)atof(arg1);
                double g = (double)atof(arg2);
                double b = (double)atof(arg3);

                printf("Setting Pixels on Line from (%i,%i) to (%i, %i) (r,g,b): (%f, %f, %f)\n",x,y,endX,endY,r,g,b);
                blit_colored_line(x, y, endX, endY, r, g, b);
                break;
            }

            case 1156:{ // ball
                if(fixed){
                    if(!pushBall(&balls, fixr, fixg, fixb)){
                        printf("Unable to Spawn Ball\n");
                    };
                    printf("Spawning Ball\n");
                    break;
                }
                if(!getNARgs(&cursor, &cursorNext, 3, &arg1, &arg2, &arg3)){
                    printf("3 Args, r, g, b, Are Required\n");
                    continue;
                };

                double r = (double)atof(arg1);
                double g = (double)atof(arg2);
                double b = (double)atof(arg3);
                if(!pushBall(&balls, r, g, b)){
                    printf("Unable to Spawn Ball\n");
                    break;
                };
                printf("Spawning Ball\n");
                break;
            }


            case 1173:{ //rain
                if(fixed){
                    
                    if(!startDigitalRain(fixr, fixg, fixb)){
                        printf("Unable to Start Digital Rain\n");
                    };
                    printf("Starting Digital Rain\n");
                    break;
                }
                if(!getNARgs(&cursor, &cursorNext, 3, &arg1, &arg2, &arg3)){
                    printf("3 Args, r, g, b, Are Required\n");
                    continue;
                };

                double r = (double)atof(arg1);
                double g = (double)atof(arg2);
                double b = (double)atof(arg3);

                if(!startDigitalRain(r, g, b)){
                    printf("Unable to Start Digital Rain\n");
                };
                printf("Starting Digital Rain\n");
                break;
            }

            case 3104:{ //rmrain
                stopDigitalRain();
                break;
            }

            case 3055:{ //rmball
                if(!popBall(&balls)){
                    printf("No Ball to Remove\n");
                    break;
                };
                printf("Removing Ball\n");
                break;

            }

            case 672:{ //fix
                fixed = 1;
                if(!getNARgs(&cursor, &cursorNext, 3, &arg1, &arg2, &arg3)){
                    printf("3 Args, r, g, b, Are Required\n");
                    continue;
                };
                fixr = (double)atof(arg1);
                fixg = (double)atof(arg2);
                fixb = (double)atof(arg3);
                printf("Fixing (r, g, b) to (%f, %f, %f)\n", fixr, fixg, fixb);
                break;

            }


            case 1236:{ //ufix
                fixed = 0;
                printf("Un-Fixing (r, g, b)\n");
                break;
            }


            case 657:{ // clr
                printf("Clearing\n");
                led_matrix_clear();
                break;
            }


            case 113:{ // q
                goto sensehat_cli_cleanup;
            }


            case 1190:{ // help
                printf( "General:\n"
                        "   Colors:\n" 
                        "      - are (r,g,b) doubles where each value is between 0.0 and 1.0\n"
                        "      - can be passed to each command, or fixed to a value using fix command\n"
                        "   Coordinates:\n"
                        "      - are (x,y) ints where each value is between 0 and 7:\n"
                        "\n"
                        "Commands:\n"
                        "   grid dir                prints a grid of leds, dir is which side the pi's ports are facing (u, d, l r)\n"
                        "   set x y r g b           set x,y coordinate to (r,g,b) doubles\n"
                        "   line x1 y1 x2 y2 r g b  set pixels from x1,y1 to x2,y2 to (r,g,b) doubles\n"
                        "   fix r g b               fix set color to (r,g,b) \n"
                        "   ufix                    unfix set color\n"
                        "   fill r g b              fill the matrix with (r,g,b) doubles\n"
                        "   clr                     clears the matrix\n" 
                        "   ball r g b              spawn bouncing ball with color (r,g,b)\n" 
                        "   rmball                  removes the last spawned ball\n" 
                        "   rain r g b              start digital rain with color (r,g,b)\n" 
                        "   rmrain                  stop digital rain\n" 
                        "   q                       quits\n");
                break;
            }
            case 1146:{ // grid

                if(!getNARgs(&cursor, &cursorNext, 1, &arg1)){
                    printf("1 Arg is required for which direction the ports of the pi are facing, one of (u, d, l, r)\n");
                    continue;
                };
                char direction = arg1[0];

                if(direction == 'r'){     printf("Ports Right ->\n");}
                else if(direction == 'l'){printf("<- Ports Left\n");}
                else if(direction == 'u'){printf("Ports Up ^\n");}
                else if(direction == 'd'){printf("Ports Down v\n");}

                for(int x = 0; x < 8; x++){
                    for(int y = 0; y < 8; y++){
                        if(direction == 'r'){
                            printf("(%i,%i) ", y, x);
                        }
                        if(direction == 'l'){
                            printf("(%i,%i) ", mathMod(7-y,8), mathMod(7-x,8));
                        }

                        if(direction == 'u'){
                            printf("(%i,%i) ", mathMod(7-x,8), y);
                        }
                        if(direction == 'd'){
                            printf("(%i,%i) ", x, mathMod(7-y,8));
                        }
                    }
                    printf("\n\n");
                }
            }


            case 1176:{ // fill
                if(!getNARgs(&cursor, &cursorNext, 3, &arg1, &arg2, &arg3)){
                    printf("3 Args, r, g, b, Are Required\n");
                    continue;
                };
                double r = (double)atof(arg1);
                double g = (double)atof(arg2);
                double b = (double)atof(arg3);

                printf("Filling with (r,g,b): (%f, %f, %f)\n",r,g,b);
                uint16_t value = rgb_double_to_hex(r,g,b);
                led_matrix_fill(value);
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

    sensehat_cli_cleanup:
    stopDigitalRain();
    stopBalls(&balls);

    //led_matrix_clear();
    destroy_sensehat_led_matrix();
    return;
}

