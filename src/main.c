#include "driver.h"
#include "helpers.h"

int main(void)
{

    printf("%i\n",smallHash("fill"));
    uint16_t * ledMatrix = getLedArr();
    
    cli(ledMatrix);
    clear(ledMatrix);
    
    unmapLedArr(ledMatrix);

    return 0;
}
