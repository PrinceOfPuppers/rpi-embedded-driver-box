#include "cli.h"

#include "demo-helpers.h"

#include "sigint-handler.h"
#include "sensehat/sensehat-cli.h"

void cli(){
    size_t buffSize = 10;
    char *buffer = malloc(10*sizeof(char));

    sensehat_cli(buff);

    free(buffer); 
}
