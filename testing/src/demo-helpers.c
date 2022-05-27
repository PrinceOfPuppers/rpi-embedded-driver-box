#include "demo-helpers.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h>

int randInt(int min, int max){
    return (rand()%(max-min)) + min;
}

double randDouble(double min, double max){
    double f = ((double)(rand())/(double)RAND_MAX)*(max-min) + min;
    return f;
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
