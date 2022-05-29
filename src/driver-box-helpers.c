#include "driver-box-helpers.h"

#include <stdio.h>

void printBin(char a){
    for (int j = 0; j < 8; j++) {
        printf("%d", !!(( a << j) & 0x80));
    }
    printf("\n");
}
