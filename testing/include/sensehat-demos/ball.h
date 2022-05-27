#ifndef "sensehat-demos/ball.h"
#define "sensehat-demos/ball.h"

#include <stdint.h>
#include <pthread.h>

#define MAX_BALLS 10

typedef struct Balls {
    int       _nextBall;
    pthread_t _btIDs[MAX_BALLS];
    int       _stopSigs[MAX_BALLS];
} Balls;

int pushBall(Balls *balls, uint16_t *map, double r, double g, double b);

int popBall(Balls *balls);

#endif
