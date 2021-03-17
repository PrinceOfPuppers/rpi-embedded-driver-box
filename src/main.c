#include "driver.h"
#include "helpers.h"

int main(void)
{

    
    uint16_t * ledMatrix = getLedArr();
    
    cli(ledMatrix);
    clear(ledMatrix);
    
    unmapLedMatrix(ledMatrix);

    return 0;
}
