#include "demo-helpers.h"
#include <stdlib.h>

int randInt(int min, int max){
    return (rand()%(max-min)) + min;
}

double randDouble(double min, double max){
    double f = ((double)(rand())/(double)RAND_MAX)*(max-min) + min;
    return f;
}

