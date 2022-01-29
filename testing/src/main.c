#include <stdio.h>
#include "sense-api.h"
#include "sense-helpers.h"

#include "cli.h"

/* ---Rainbow Effect--- (requires math library to be linked with -lm)

#include <math.h>
#include <time.h>
void rainbow(uint16_t * ledMatrix){
    double t = 0.0;
    double r;
    double g;
    double b;
    while(1){
        for(int x = 0; x<8; x++){
            for(int y = 0; y<8; y++){
                r = 0.5*sin( M_PI*((double)x /14.0 + t)) + 0.5;
                g = 0.5*sin( M_PI*(((double)y+(double)x) /14.0 + t + 2./3.)) + 0.5;
                b = 0.5*sin( M_PI*( (double)y/14.0 + t + 4./3.)) + 0.5;

                setVal(ledMatrix,x,y,rgbDoubleToHex(r,g,b) );
            } 
        }
        t+=0.001;
        t = fmod(t,2);
        usleep(500);
    }
}

*/

int main(void)
{

    uint16_t * ledMatrix = getLedArr();

    cli(ledMatrix);
    clear(ledMatrix);
    
    unmapLedArr(ledMatrix);

    return 0;
}
