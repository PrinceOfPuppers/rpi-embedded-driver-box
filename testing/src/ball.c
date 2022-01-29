#include <math.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "ball.h"
#include "sense-api.h"
#include "sense-helpers.h"
#include "demo-helpers.h"


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



void bouncyBall(uint16_t *map, int *stopSig, double r, double g, double b){
    double x = (double)randInt(0, 7);
    double y = (double)randInt(0, 7);

    double startVelX = randDouble(2e-3, 6e-3);
    double startVelY = randDouble(2e-3, 6e-3);
    if(randInt(0,1)){
        startVelX  = -startVelX;
    }
    if(randInt(0,1)){
        startVelY  = -startVelY;
    }

    int frameTime = 200;
    while(!*stopSig){
        blitpixel(map, x, y, r, g, b);

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
    blitpixel(map, x, y, 0.0, 0.0, 0.0);
}

typedef struct BallThreadData {
    uint16_t *map;
    int      *stopSig;
    double r;
    double g;
    double b;
} BallThreadData;

void *ballThread(void *_b){
    BallThreadData *ballData = _b;

    uint16_t *map     = ballData->map;
    int      *stopSig = ballData->stopSig;
    double     r       = ballData->r;
    double     g       = ballData->g;
    double     b       = ballData->b;

    free(ballData);
    bouncyBall(map, stopSig, r, g, b);

    return NULL;
}

int spawnBall(uint16_t *map, pthread_t *tId, int *stopSig, double r, double g, double b){
    BallThreadData *ballData = malloc(sizeof(BallThreadData));
    *stopSig = 0;

    ballData->map      = map;
    ballData->stopSig  = stopSig;
    ballData->r        = r;
    ballData->g        = g;
    ballData->b        = b;

    int error = pthread_create(tId, NULL, ballThread, (void*)ballData);
    if (error) {
        free(ballData);
        return 0;
    }

    return 1;
}

int pushBall(Balls *balls, uint16_t *map, double r, double g, double b){
    if(balls->_nextBall > MAX_BALLS){
        return 0;
    }
    pthread_t *tId     = &(balls->_btIDs[balls->_nextBall]);
    int *stopSig = &(balls->_stopSigs[balls->_nextBall]);
    spawnBall(map, tId, stopSig, r, g, b);
    balls->_nextBall += 1;
    return 1;
}

int popBall(Balls *balls){
    if(balls->_nextBall == 0){
        return 0;
    }
    (balls->_stopSigs[balls->_nextBall - 1]) = 1;
    balls->_nextBall -= 1;
    return 1;
}
