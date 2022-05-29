#include "sigint-handler.h"

#include <signal.h>

static void sigint_handler(){
    sigint_triggered = 1;
}

void init_sigint_handler(){
    sigint_triggered = 0;
    struct sigaction int_handler = {.sa_handler=sigint_handler};
    sigaction(SIGINT,&int_handler,0);
}

void destroy_sigint_handler(){
    signal(SIGINT, SIG_DFL);
}

