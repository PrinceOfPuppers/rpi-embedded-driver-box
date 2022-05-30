#include "nunchuk-demos/nunchuk-demos.h"

#include <stdio.h>

#include "nunchuk-demos/nunchuk-led-bar-graph.h"
#include "nunchuk-demos/nunchuk-joystick-sensehat.h"
#include "nunchuk-demos/nunchuk-acc-sensehat.h"
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

    char *arg1;

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

            case 3166:{ // joyhat
                if(!getNARgs(&cursor, &cursorNext, 1, &arg1)){
                    printf("1 Arg is required for which direction the ports of the pi are facing, one of (u, d, l, r)\n");
                    continue;
                };
                char direction = arg1[0];

                start_nunchuk_joystick_sensehat(direction);
                break;
            }

            case 6429:{ // rmjoyhat
                stop_nunchuk_acc_sensehat();
                break;
            }

            case 3067:{ // acchat
                if(!getNARgs(&cursor, &cursorNext, 1, &arg1)){
                    printf("1 Arg is required for which direction the ports of the pi are facing, one of (u, d, l, r)\n");
                    continue;
                };
                char direction = arg1[0];

                start_nunchuk_acc_sensehat(direction);
                break;
            }

            case 6188:{ // rmacchat
                stop_nunchuk_acc_sensehat();
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
                        "   bar          using the nunchuk's joystick, control an led bar graph on gpio pins specified in led-bar-graph.h\n"
                        "   rmbar        stops bar\n"
                        "   joyhat dir   use the joystick to move a ball on the sensehat's led matrix, dir is where the pi's ports are facing (u, d, l r)\n"
                        "   rmjoyhat     stops joyhat\n"
                        "   acchat dir   use tilt controls to move a ball on the sensehat's led matrix, dir is where pi's ports are facing (u, d, l r)\n"
                        "   rmacchat     stops acchat\n"
                        "   data         get a snapshot of the nunchuks state\n"
                        "   rawdata      get a snapshot of the raw data being sent by the nunchuk\n"
                        "   q            quits to previous menu\n");
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
    stop_nunchuk_joystick_sensehat();
    destroy_nunchuk();
}
