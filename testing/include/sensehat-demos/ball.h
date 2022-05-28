#ifndef SENSEHAT_DEMOS_BALL_H_
#define SENSEHAT_DEMOS_BALL_H_

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

void stopBalls(Balls *balls);

#endif
