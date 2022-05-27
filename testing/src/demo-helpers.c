#include "demo-helpers.h"
#include <stdlib.h>

int randInt(int min, int max){
    return (rand()%(max-min)) + min;
}

double randDouble(double min, double max){
    double f = ((double)(rand())/(double)RAND_MAX)*(max-min) + min;
    return f;
}

void blitpixel(uint16_t *map, double x, double y, double r, double g, double b){
    double px = trunc(x);
    double py = trunc(y);
    int pxi = (int)px;
    int pyi = (int)py;


    double opacity = (1.0 + px - x)*(1.0 + py - y);
    setVal(map,pxi,pyi,rgbDoubleToHex(opacity*r,opacity*g,opacity*b));

    opacity = (x - px)*(py + 1.0 - y);
    setVal(map, (pxi + 1),pyi,rgbDoubleToHex(opacity*r,opacity*g,opacity*b));

    opacity = (px + 1.0 - x)*(y - py);
    setVal(map,pxi, (pyi + 1),rgbDoubleToHex(opacity*r,opacity*g,opacity*b));

    opacity = (x-px)*(y-py);
    setVal(map,(pxi + 1),(pyi+1),rgbDoubleToHex(opacity*r,opacity*g,opacity*b));
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
