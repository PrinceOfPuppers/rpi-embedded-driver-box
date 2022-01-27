#include "cli.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

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


void blitpixel(uint16_t *map, float x, float y, float r, float g, float b){
    float px = trunc(x);
    float py = trunc(y);
    int pxi = (int)px;
    int pyi = (int)py;


    float opacity = (1.0 + px - x)*(1.0 + py - y);
    setVal(map,pxi,pyi,rgbFloatToHex(opacity*r,opacity*g,opacity*b));

    opacity = (x - px)*(py + 1.0 - y);
    setVal(map, (pxi + 1),pyi,rgbFloatToHex(opacity*r,opacity*g,opacity*b));

    opacity = (px + 1.0 - x)*(y - py);
    setVal(map,pxi, (pyi + 1),rgbFloatToHex(opacity*r,opacity*g,opacity*b));

    opacity = (x-px)*(y-py);
    setVal(map,(pxi + 1),(pyi+1),rgbFloatToHex(opacity*r,opacity*g,opacity*b));
}
int randInt(int min, int max){
    return (rand()%(max-min)) + min;
}


void bouncyBall(uint16_t *map){
    float x = (float)randInt(0, 7);
    float y = (float)randInt(0, 7);

    float startVelX = (float)randInt(1, 3) / 500.0 + 0.001;
    float startVelY = (float)randInt(1, 3) / 500.0 + 0.0003;

    int frameTime = 200;
    while(1){
        //clear(map);
        blitpixel(map, x, y, 1.0, 1.0, 1.0);

        x += startVelX;
        y += startVelY;

        if(x < 0){
            x = 0;
            startVelX = - startVelX;
        }
        if(x > 7){
            x = 7;
            startVelX = - startVelX;
        }

        if(y < 0){
            y = 0;
            startVelY = - startVelY;
        }
        if(y > 7){
            y = 7;
            startVelY = - startVelY;
        }

        usleep(frameTime);
    }
}




void cli(uint16_t * map){

    time_t t;
    srand((unsigned) time(&t));

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
            case 1226:{
                printf("Playing Ball");
                bouncyBall(map);
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


            case 1306:{ //ufix
                fixed = 0;
                printf("Un-Fixing (r, g, b)\n");
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
                        "   set x y r g b            set x,y coordinate to (r,g,b) floats (color overrided by fix)\n"
                        "   line x1 y1 x2 y2 r g b   set pixels from x1,y1 to x2,y2 to (r,g,b) floats (color overrided by fix)\n"
                        "   fix r g b                fix set color to (r,g,b) \n"
                        "   ufix                     unfix set color\n"
                        "   fill r g b               fill the matrix with (r,g,b) floats\n"
                        "   clr                      clears the matrix\n" 
                        "   ball                     play ball bouncing\n" 
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
