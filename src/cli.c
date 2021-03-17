#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"
#include "driver.h"
#include "helpers.h"

int argGenerator(char **cursor, char **cursorNext){
    while((**cursorNext)==' '){ // skips past empty spaces
        (*cursorNext)++;
    }

    if((**cursorNext)=='\0' || (**cursorNext)=='\n'){ 
        return 0;
    }

    *cursor = *cursorNext;

    while (**cursorNext != ' ')
    {
        if ( (**cursorNext) == '\0' || (**cursorNext)=='\n' )  {
            return 1;
        }

        (*cursorNext)++;
    }

    **cursorNext = '\0';
    (*cursorNext)++;

    return 1;
}



void cli(uint16_t * map){
    printf("Sense Hat Interactive Mode:\n");
    size_t buffSize = 10;
    char *buffer = malloc(10*sizeof(char));
    //buffSize = parseArgs(buffer,buffSize);



    char * cursor    ;
    char * cursorNext;
    //int i;

    while(1){
        printf("~ ");
        buffSize = getline(&buffer,&buffSize,stdin);
        cursor     = buffer;
        cursorNext = buffer;

        argGenerator(&cursor,&cursorNext);
        switch(smallHash(cursor)){

            case 665:{ // set
                argGenerator(&cursor,&cursorNext);
                int x = atoi(cursor);
                argGenerator(&cursor,&cursorNext);
                int y = atoi(cursor);
                argGenerator(&cursor,&cursorNext);
                float r = (float)atof(cursor);
                argGenerator(&cursor,&cursorNext);
                float g = (float)atof(cursor);
                argGenerator(&cursor,&cursorNext);
                float b = (float)atof(cursor);
                argGenerator(&cursor,&cursorNext);

                printf("x: %i y: %i color(rgb): %f %f %f\n",x,y,r,g,b);
                setVal(map,x,y,rgbFloatToHex(r,g,b));
                break;
            }
            case 657:{ // clear
                clear(map);
                break;
            }
            case 133:{ //quit
                return;
            }
            case 1260:{ // help
                printf( "commands:\n"
                        "   set x y colorNum   set x,y coordinate to colorNum (int)\n"
                        "   clear              clears matrix\n" 
                        "   q                  quits\n");
                break;
            }
        }

    }

    free(buffer); 
    return;
}
