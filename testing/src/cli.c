#include "cli.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sense-api.h"
#include "sense-helpers.h"

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

                printf("Setting x: %i y: %i to (r,g,b): (%f, %f, %f)\n",x,y,r,g,b);
                setVal(map,x,y,rgbFloatToHex(r,g,b));
                break;
            }
            case 707:{ // clr
                printf("Clearing\n");
                clear(map);
                break;
            }
            case 133:{ //quit
                return;
            }
            case 1260:{ // help
                printf( "commands:\n"
                        "   set x y r g b   set x,y coordinate to (r,g,b) floats\n"
                        "   fill r g b      fill the matrix with (r,g,b) floats\n"
                        "   clr             clears the matrix\n" 
                        "   q               quits\n");
                break;
            }
            case 1176:{ // fill
                argGenerator(&cursor,&cursorNext);
                float r = (float)atof(cursor);
                argGenerator(&cursor,&cursorNext);
                float g = (float)atof(cursor);
                argGenerator(&cursor,&cursorNext);
                float b = (float)atof(cursor);

                printf("Filling with (r,g,b): (%f, %f, %f)\n",r,g,b);
                uint16_t value = rgbFloatToHex(r,g,b);
                fill(map,value);
                break;
            }
        }

    }

    free(buffer); 
    return;
}
