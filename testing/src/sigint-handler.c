#include "sigint-handler.h"

#include <signal.h>

void sigint_handler(){
    sigint_triggered = 1;
}

void init_sigint_handler(){
    sigint_triggered = 0;
    signal(SIGINT, sigint_handler);
}

void destroy_sigint_handler(){
    signal(SIGINT, SIG_DFL);
}

