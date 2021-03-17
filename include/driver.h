#pragma once

#include <stdint.h>

#define LEDS 64
#define FILESIZE (LEDS * sizeof(uint16_t))

uint16_t * getLedArr();

void unmapLedArr(uint16_t *Arr);

#define setVal(ledArr, x, y, val) ledArr[8*y+x] = val;

#define clear(Arr) memset(Arr, 0, FILESIZE);

