#include <stdio.h>

#include "sigint-handler.h"
#include "cli.h"

int main(void)
{
    init_sigint_handler();

    cli();
    
    return 0;
}
