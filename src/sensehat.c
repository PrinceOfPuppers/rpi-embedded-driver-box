#include "sensehat.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <math.h>

#define DEV_PATH_FMT "/dev/fb%c"

static int fd;

uint16_t *getLedArr(){
    uint16_t *ledArr;
    struct fb_fix_screeninfo fix_info;

    char * devPath = malloc(strlen(DEV_PATH_FMT));
    for(int i = 0; i<3; i++){
        char numChar = i + '0';
        sprintf(devPath, DEV_PATH_FMT, numChar);

        // open frame buffer
        fd = open(devPath, O_RDWR);
        if (fd == -1) {
            continue;
        }

        // read framebuffer info for device so we can check to see if its the correct one
        if (ioctl(fd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
            close(fd);
            continue;
        }

        // check the frame buffers id to ensure it matches the pi sense hat's frame buffer id
        if (strcmp(fix_info.id, "RPi-Sense FB") != 0) {
            close(fd);
            continue;
        }

        // map frame buffer into virtual memory
        ledArr = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ledArr == MAP_FAILED) {
            close(fd);
            perror("Error mmapping the file");
            exit(EXIT_FAILURE);
        }
        free(devPath);
        return ledArr;
    }
    fprintf(stderr,"Error Getting Sense Hat");
    exit(EXIT_FAILURE);

}


void unmapLedArr(uint16_t * ledArr){
    // unmap virtual memory corrisponding to frame buffer
    if (munmap(ledArr, FILESIZE) == -1) {
	    perror("Error un-mmapping the file");
    }
    close(fd);
}

uint16_t rgbIntToHex(uint16_t r, uint16_t g, uint16_t b){

    r = r << 11;

    g = g << 5;

    return r|g|b;
}

uint16_t rgbDoubleToHex(double r, double g, double b){
    return rgbIntToHex((uint16_t)(r*31),(uint16_t)(g*63),(uint16_t)(b*31));
}

void blitpixel(uint16_t *map, double x, double y, double r, double g, double b){
    double px = trunc(x);
    double py = trunc(y);
    int pxi = (int)px;
    int pyi = (int)py;


    double opacity = (1.0 + px - x)*(1.0 + py - y);
    setVal(map,pxi,pyi,rgbDoubleToHex(opacity*r,opacity*g,opacity*b));

    opacity = (x - px)*(py + 1.0 - y);
    setVal(map, (pxi + 1),pyi,rgbDoubleToHex(opacity*r,opacity*g,opacity*b));

    opacity = (px + 1.0 - x)*(y - py);
    setVal(map,pxi, (pyi + 1),rgbDoubleToHex(opacity*r,opacity*g,opacity*b));

    opacity = (x-px)*(y-py);
    setVal(map,(pxi + 1),(pyi+1),rgbDoubleToHex(opacity*r,opacity*g,opacity*b));
}

