#include "nunchuk-demos/nunchuk-demos.h"

#include <stdio.h>

#include "nunchuk-demos/nunchuk-led-bar-graph.h"
#include "nunchuk-demos/nunchuk-sensehat.h"
#include "nunchuk.h"
#include "demo-helpers.h"
#include "sigint-handler.h"


void nunchuk_cli(char *buffer, size_t *buffSize){
    if(!init_nunchuk()){
        printf("error getting nunchuk\n");
        return;
    };

    start_nunchuk_polling();

    char *cursor    ;
    char *cursorNext;

    while(!sigint_triggered){
        printf("\nNunchuk Demos (type help for options):\n");
        printf("~ ");
        *buffSize = getline(&buffer, buffSize, stdin);
        if(sigint_triggered){break;}
        cursor     = buffer;
        cursorNext = buffer;

        argGenerator(&cursor,&cursorNext);
        printf("%s: %i\n", cursor, smallHash(cursor));
        switch(smallHash(cursor)){

            case 634:{ // bar
                start_nunchuk_led_bar_graph();
                break;
            }

            case 1909:{ // rmbar
                stop_nunchuk_led_bar_graph();
                break;
            }

            case 3276:{ // matrix
                start_nunchuk_sensehat();
                break;
            }

            case 6615:{ // rmmatrix
                stop_nunchuk_sensehat();
                break;
            }

            case 1127:{ // data
                print_nunchuk();
                break;
            }


            case 4381:{ // rawdata
                print_nunchuk_raw();
                break;
            }


            case 113:{ // q
                goto nunchuk_cli_clean_up;
            }


            case 1190:{ // help
                printf( "Commands:\n"
                        "   bar       using the nunchuk, control an led bar graph on gpio pins specified in led-bar-graph.h\n"
                        "   rmbar     stops bar\n"
                        "   data      get a snapshot of the nunchuks state\n" 
                        "   rawdata   get a snapshot of the raw data being sent by the nunchuk\n" 
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
    
    nunchuk_cli_clean_up:
    stop_nunchuk_led_bar_graph();
    stop_nunchuk_sensehat();
    destroy_nunchuk();
}
