#include "driver-box-helpers.h"

int primes[5] = {1,2,3,5,7};

int smallHash(char * str){
    int sum = 0;
    int i=0;
    while(str[i] != '\0'){
        sum+=primes[i]*str[i];
        i++;
    }
    return sum;

}



