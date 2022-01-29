#pragma once

int randInt(int min, int max);

double randDouble(double min, double max);

#define mathMod(a, b)        \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    (_a %_b + _b)%_b;        \
})

#define mathMax(a,b)         \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})
