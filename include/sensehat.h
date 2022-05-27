#ifndef SENSEHAT_H_
#define SENSEHAT_H_

#include <stdint.h>
#include <string.h>

#define LEDS 64
#define FILESIZE (LEDS * sizeof(uint16_t))

uint16_t *getLedArr();

void unmapLedArr(uint16_t *Arr);

#define setVal(ledArr, x, y, val) ledArr[8*y+x] = val;

#define clear(Arr) memset(Arr, 0, FILESIZE);

#define fill(Arr,value) for(int i=0; i<LEDS; i+=1){ Arr[i] = value; }

// 16 bit color conversions
uint16_t rgbIntToHex(uint16_t r, uint16_t g, uint16_t b);

uint16_t rgbDoubleToHex(double r, double g, double b);

void blitpixel(uint16_t *map, double x, double y, double r, double g, double b);

#endif

