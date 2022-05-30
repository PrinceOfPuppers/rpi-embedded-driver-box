#pragma once

int randInt(int min, int max);

double randDouble(double min, double max);

void port_right_convert(double x, double y, double *nx, double *ny);
void port_left_convert(double x, double y, double *nx, double *ny);
void port_up_convert(double x, double y, double *nx, double *ny);
void port_down_convert(double x, double y, double *nx, double *ny);

#define mathMod(a, b)        \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    (_a %_b + _b)%_b;        \
})

#define mathMax(a ,b)        \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})


// argument parsing
int smallHash(char * str);

int getNARgs(char **cursor, char **cursorNext, int n, ...);

int argGenerator(char **cursor, char **cursorNext);
