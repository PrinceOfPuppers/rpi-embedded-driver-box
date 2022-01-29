#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>


#include "digital-rain.h"
#include "sense-api.h"
#include "sense-helpers.h"
#include "demo-helpers.h"


double tailOpacityCurve(double y, int ledY, double tailLen){
    if (ledY > y){
        return mathMax(1 - fabs(((double)ledY-8-y) / tailLen), 0.0); // handle screenwrapping
    }

    return mathMax(1 - fabs(((double)ledY-y) / tailLen), 0.0);
}

void setDRainDrop(uint16_t *map, int x, double y, int tailLen, double r, double g, double b){
    int headY = (int)trunc(y);

    for(int i = 0; i < tailLen; i ++){
        int ledY = mathMod(headY - i, 8);
        double opacity = tailOpacityCurve(y, ledY, tailLen);
        setVal(map, x, (7-ledY), rgbDoubleToHex(opacity*r, opacity*g, opacity*b));
    }
}

void digitalRain(uint16_t *map, int *stopSig, double r, double g, double b){
    double tailLen = 4.0;
    double dropYs[8];
    double dropVels[8];


    for(int i = 0; i < 8; i++){
        dropYs[i]  = randDouble(0.0, 7.0);
        dropVels[i] = randDouble(8e-3, 3e-2);
    }

    while(!*stopSig){
        for(int i = 0; i < 8; i++){
            dropYs[i] += dropVels[i];
            dropYs[i] = fmod(dropYs[i], 7.99999999);
            setDRainDrop(map, i, dropYs[i], tailLen, r, g, b);
            usleep(200);
        }
    }

    for(int i = 0; i < 8; i++){
        setDRainDrop(map, i, dropYs[i], tailLen, 0.0, 0.0, 0.0);
    }
}


typedef struct DigitalRainThreadData {
    uint16_t *map;
    int      *stopSig;
    double r;
    double g;
    double b;
} DigitalRainThreadData;

void *digitalRainThread(void *_b){
    DigitalRainThreadData  *dRainData= _b;

    uint16_t *map      = dRainData->map;
    int      *stopSig  = dRainData->stopSig;
    double   r         = dRainData->r;
    double   g         = dRainData->g;
    double   b         = dRainData->b;

    free(dRainData);
    digitalRain(map, stopSig, r, g, b);

    return NULL;
}

pthread_t tId;
int _stopSig;


int inited;

void stopDigitalRain(){
    if(inited && !_stopSig){
        _stopSig = 1;
        pthread_join(tId, NULL);
    }
}

int startDigitalRain(uint16_t *map, double r, double g, double b){
    stopDigitalRain();
    _stopSig = 0;

    DigitalRainThreadData *dRainData = malloc(sizeof(DigitalRainThreadData));
    dRainData->map      = map;
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


