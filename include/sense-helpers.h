#pragma once
#include <stdint.h>

int smallHash(char * str);

// 16 bit color conversions
uint16_t rgbIntToHex(uint16_t r, uint16_t g, uint16_t b);

uint16_t rgbDoubleToHex(double r, double g, double b);