#pragma once
#include <stdint.h>

#define MAX_BALLS 10

typedef struct Balls {
    int       _nextBall;
    pthread_t _btIDs[MAX_BALLS];
    int       _stopSigs[MAX_BALLS];
} Balls;

int pushBall(Balls *balls, uint16_t *map, float r, float g, float b);

int popBall(Balls *balls);
