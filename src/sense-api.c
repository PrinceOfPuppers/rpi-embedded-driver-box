#include "sense-api.h"

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

#define DEV_PATH_FMT "/dev/fb%c"

int fd;

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
