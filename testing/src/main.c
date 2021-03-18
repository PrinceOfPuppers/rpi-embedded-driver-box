#include <stdio.h>
#include "sense-api.h"
#include "sense-helpers.h"

#include "cli.h"
int main(void)
{

    uint16_t * ledMatrix = getLedArr();

    //raindbow effect
    //#include <math.h>
    //#include<time.h>
    //double t = 0.0;
    //double r;
    //double g;
    //double b;
    //while(1){
    //    for(int x = 0; x<8; x++){
    //        for(int y = 0; y<8; y++){
    //            r = 0.5*sin( M_PI*((double)x /14.0 + t)) + 0.5;
    //            g = 0.5*sin( M_PI*(((double)y+(double)x) /14.0 + t + 2./3.)) + 0.5;
    //            b = 0.5*sin( M_PI*( (double)y/14.0 + t + 4./3.)) + 0.5;
//
    //            setVal(ledMatrix,x,y,rgbFloatToHex(r,g,b) );
    //        } 
    //    }
    //    t+=0.001;
    //    t = fmod(t,2);
    //    usleep(500);
    //}

    cli(ledMatrix);
    clear(ledMatrix);
    
    unmapLedArr(ledMatrix);

    return 0;
}
