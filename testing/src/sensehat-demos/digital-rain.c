#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>


#include "sensehat-demos/digital-rain.h"
#include "sensehat.h"
#include "driver-box-helpers.h"
#include "demo-helpers.h"

#include "sigint-handler.h"


static double tailOpacityCurve(double y, int ledY, double tailLen){
    if (ledY > y){
        return mathMax(1 - fabs(((double)ledY-8-y) / tailLen), 0.0); // handle screenwrapping
    }

    return mathMax(1 - fabs(((double)ledY-y) / tailLen), 0.0);
}

static void setDRainDrop(int x, double y, int tailLen, double r, double g, double b){
    int headY = (int)trunc(y);

    for(int i = 0; i < tailLen; i ++){
        int ledY = mathMod(headY - i, 8);
        double opacity = tailOpacityCurve(y, ledY, tailLen);
        led_matrix_set_val(x, (7-ledY), rgb_double_to_hex(opacity*r, opacity*g, opacity*b));
    }
}

static void digitalRain(int *stopSig, double r, double g, double b){
    double tailLen = 4.0;
    double dropYs[8];
    double dropVels[8];


    for(int i = 0; i < 8; i++){
        dropYs[i]  = randDouble(0.0, 7.0);
        dropVels[i] = randDouble(8e-3, 3e-2);
    }

    while(!*stopSig &&!sigint_triggered){
        for(int i = 0; i < 8; i++){
            dropYs[i] += dropVels[i];
            dropYs[i] = fmod(dropYs[i], 7.99999999);
            setDRainDrop(i, dropYs[i], tailLen, r, g, b);
            usleep(200);
        }
    }

    for(int i = 0; i < 8; i++){
        setDRainDrop(i, dropYs[i], tailLen, 0.0, 0.0, 0.0);
    }
}


typedef struct DigitalRainThreadData {
    int      *stopSig;
    double r;
    double g;
    double b;
} DigitalRainThreadData;

static void *digitalRainThread(void *_b){
    DigitalRainThreadData  *dRainData= _b;

    int      *stopSig  = dRainData->stopSig;
    double   r         = dRainData->r;
    double   g         = dRainData->g;
    double   b         = dRainData->b;

    free(dRainData);
    digitalRain(stopSig, r, g, b);

    return NULL;
}

static pthread_t tId;
static int _stopSig;

static int inited;

void stopDigitalRain(){
    if(inited && !_stopSig){
        _stopSig = 1;
        pthread_join(tId, NULL);
    }
}

int startDigitalRain(double r, double g, double b){
    stopDigitalRain();
    _stopSig = 0;

    DigitalRainThreadData *dRainData = malloc(sizeof(DigitalRainThreadData));
    dRainData->stopSig  = &_stopSig;
    dRainData->r        = r;
    dRainData->g        = g;
    dRainData->b        = b;

    int error = pthread_create(&tId, NULL, digitalRainThread, (void*)dRainData);
    if (error) {
        free(dRainData);
        return 0;
    }
    inited = 1;

    return 1;
}


