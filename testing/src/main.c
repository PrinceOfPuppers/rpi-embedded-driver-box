#include "sigint-handler.h"
#include "cli.h"

int main()
{
    init_sigint_handler();

    cli();

    return 0;
}
