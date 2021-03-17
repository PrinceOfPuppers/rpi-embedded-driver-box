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

#include "driver.h"

#define DEV_PATH "/dev/fb1"

int fd;


uint16_t * getLedArr(){
    uint16_t *ledArr;
    struct fb_fix_screeninfo fix_info;

    // open frame buffer
    fd = open(DEV_PATH, O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Error When Opening %s : ",DEV_PATH);
	    perror("");
	    exit(EXIT_FAILURE);
    }

    // read framebuffer info for device so we can check to see if its the correct one
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
	    perror("Unable to get device info ");
	    close(fd);
	    exit(EXIT_FAILURE);
    }

    // check the frame buffers id to ensure it matches the pi sense hat's frame buffer id
    if (strcmp(fix_info.id, "RPi-Sense FB") != 0) {
	    fprintf(stderr,"Error: Device id %s Found Instead of 'RPi-Sense FB'\n");
        fprintf(stderr,"Frame Buffer Length: %i Expected: %i \n",fix_info.smem_len, FILESIZE);
	    close(fd);
	    exit(EXIT_FAILURE);
    }

    // map frame buffer into virtual memory
    ledArr = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ledArr == MAP_FAILED) {
	    close(fd);
	    perror("Error mmapping the file");
	    exit(EXIT_FAILURE);
    }
    return ledArr;
}


void unmapLedArr(uint16_t * ledArr){
    // unmap virtual memory corrisponding to frame buffer
    if (munmap(ledArr, FILESIZE) == -1) {
	    perror("Error un-mmapping the file");
    }
    close(fd);
}