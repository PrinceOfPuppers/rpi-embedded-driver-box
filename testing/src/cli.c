#include "cli.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "sense-api.h"
#include "sense-helpers.h"

#include "ball.h"

int argGenerator(char **cursor, char **cursorNext){
    while((**cursorNext)==' '){ // skips past empty spaces
        (*cursorNext)++;
    }

    if ((**cursorNext)=='\n'){
        **cursorNext = '\0';
        return 0;
    }
    if ( (**cursorNext) == '\0' )  {
        return 0;
    }

    *cursor = *cursorNext;

    while (**cursorNext != ' ')
    {
        if ((**cursorNext)=='\n'){
            **cursorNext = '\0';
            return 1;
        }
        if ( (**cursorNext) == '\0' )  {
            return 1;
        }

        (*cursorNext)++;
    }

    **cursorNext = '\0';
    (*cursorNext)++;

    return 1;
}

int rasterLineGenerator(int *x, int *y, int endX, int endY){
    if(*x == endX && *y == endY){ return 0; }

    float diffX = (float)(endX - *x);
    float diffY = (float)(endY - *y);

    if(endX != *x){
        float slope = diffY/diffX;
        if(slope < -0.25 || slope > 0.25){
            if (diffY > 0){
                *y+=1;
            }
            else{
                *y-=1;
            }
        }

        if(slope > -4 && slope < 4){
            if (diffX > 0){
                *x+=1;
            }
            else{
                *x-=1;
            }
        }
        return 1;
    }

    if (diffY > 0){
        *y+=1;
    }
    else{
        *y-=1;
    }
    return 1;
}



int getNARgs(char **cursor, char **cursorNext, int n, ...){
    
    va_list v;
    va_start(v, n);

    char **arg;
    for (int i = 0; i < n; i++)
    {
        arg = va_arg(v, char **);
        if(!argGenerator(cursor,cursorNext)){
            return 0;
        }
        *arg = *cursor;
    }
    return 1;
}

void colorLine(uint16_t * map, int startX, int startY, int endX, int endY, float r, float g, float b){
    printf("Setting Pixels on Line from (%i,%i) to (%i, %i) (r,g,b): (%f, %f, %f)\n",startX,startY,endX,endY,r,g,b);
    setVal(map,startX,startY,rgbFloatToHex(r,g,b));
    while(rasterLineGenerator(&startX, &startY, endX, endY)){
        setVal(map,startX,startY,rgbFloatToHex(r,g,b));
    }
}





void cli(uint16_t * map){
    time_t t;
    srand((unsigned) time(&t));

    Balls balls;
    balls._nextBall = 0;

    printf("Sense Hat Interactive Mode:\n");
    size_t buffSize = 10;
    char *buffer = malloc(10*sizeof(char));

    char * cursor    ;
    char * cursorNext;

    int fixed = 0;

    float fixr = 0.0;
    float fixg = 0.0;
    float fixb = 0.0;


    char *arg1, *arg2, *arg3, *arg4;

    while(1){
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
                    setVal(map,x,y,rgbFloatToHex(fixr,fixg,fixb));
                    continue;
                }

                if(!getNARgs(&cursor, &cursorNext, 3, &arg1, &arg2, &arg3)){
                    printf("5 Args, x, y, r, g, b, Are Required\n");
                    continue;
                };
                float r = (float)atof(arg1);
                float g = (float)atof(arg2);
                float b = (float)atof(arg3);

                printf("Setting x: %i y: %i to (r,g,b): (%f, %f, %f)\n",x,y,r,g,b);
                setVal(map,x,y,rgbFloatToHex(r,g,b));
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
                    colorLine(map, x, y, endX, endY, fixr, fixg, fixb);
                    continue;
                }
                if(!getNARgs(&cursor, &cursorNext, 3, &arg1, &arg2, &arg3)){
                    printf("7 Args, startX, startY, endX, endY, r, g, b, Are Required\n");
                    continue;
                };
                float r = (float)atof(arg1);
                float g = (float)atof(arg2);
                float b = (float)atof(arg3);

                colorLine(map, x, y, endX, endY, r, g, b);
                break;
            }
            case 1156:{ // ball
                if(fixed){
                    if(!pushBall(&balls, map, fixr, fixg, fixb)){
                        printf("Unable to Spawn Ball\n");
                    };
                    printf("Spawning Ball\n");
                    break;
                }
                if(!getNARgs(&cursor, &cursorNext, 3, &arg1, &arg2, &arg3)){
                    printf("3 Args, r, g, b, Are Required\n");
                    continue;
                };

                float r = (float)atof(arg1);
                float g = (float)atof(arg2);
                float b = (float)atof(arg3);
                if(!pushBall(&balls, map, r, g, b)){
                    printf("Unable to Spawn Ball\n");
                    break;
                };
                printf("Spawning Ball\n");
                break;
            }

            case 1867:{ //rmball
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
                fixr = (float)atof(arg1);
                fixg = (float)atof(arg2);
                fixb = (float)atof(arg3);
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
                clear(map);
                break;
            }


            case 113:{ //quit
                return;
            }


            case 1190:{ // help
                printf( "General:\n"
                        "   Colors:\n" 
                        "      - are (r,g,b) floats where each value is between 0.0 and 1.0\n"
                        "      - can be passed to each command, or fixed to a value using fix command\n"
                        "   Coordinates:\n"
                        "      - are (x,y) ints where each value is between 0 and 7:\n"
                        "\n"
                        "Commands:\n"
                        "   set x y r g b            set x,y coordinate to (r,g,b) floats\n"
                        "   line x1 y1 x2 y2 r g b   set pixels from x1,y1 to x2,y2 to (r,g,b) floats\n"
                        "   fix r g b                fix set color to (r,g,b) \n"
                        "   ufix                     unfix set color\n"
                        "   fill r g b               fill the matrix with (r,g,b) floats\n"
                        "   clr                      clears the matrix\n" 
                        "   ball r g b               spawn bouncing ball with color (r,g,b)\n" 
                        "   rmball                    removes the last spawned ball\n" 
                        "   q                        quits\n");
                break;
            }


            case 1176:{ // fill
                if(!getNARgs(&cursor, &cursorNext, 3, &arg1, &arg2, &arg3)){
                    printf("3 Args, r, g, b, Are Required\n");
                    continue;
                };
                float r = (float)atof(arg1);
                float g = (float)atof(arg2);
                float b = (float)atof(arg3);

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
