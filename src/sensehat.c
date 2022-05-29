#include "sensehat.h"

#include <stdio.h>
#include <string.h>
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

static int led_matrix_fd;

static int led_matrix_inited;
static uint16_t *led_matrix;

int init_sensehat_led_matrix(){
    if(led_matrix_inited){
        return 1;
    }
    led_matrix_inited = 1;

    struct fb_fix_screeninfo fix_info;

    char * devPath = malloc(strlen(DEV_PATH_FMT));
    for(int i = 0; i<3; i++){
        char numChar = i + '0';
        sprintf(devPath, DEV_PATH_FMT, numChar);

        led_matrix_fd = open(devPath, O_RDWR);
        if (led_matrix_fd == -1) {
            continue;
        }

        // read framebuffer info
        if (ioctl(led_matrix_fd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
            close(led_matrix_fd);
            continue;
        }

        // check the frame buffers id
        if (strcmp(fix_info.id, "RPi-Sense FB") != 0) {
            close(led_matrix_fd);
            continue;
        }

        // map frame buffer into virtual memory
        led_matrix = mmap(NULL, LED_MATRIX_FILESIZE , PROT_READ | PROT_WRITE, MAP_SHARED, led_matrix_fd, 0);
        if (led_matrix == MAP_FAILED) {
            close(led_matrix_fd);
            perror("error mmapping sense hat led matrix");
            led_matrix_inited = 0;
            return 0;
        }
        free(devPath);
        led_matrix_inited = 0;
        return 1;
    }
    fprintf(stderr, "error getting sensehat led matrix (is it connected?)");
    return 0;
}

void destroy_sensehat_led_matrix(){
    // unmap virtual memory corrisponding to frame buffer
    if (munmap(led_matrix, LED_MATRIX_FILESIZE ) == -1) {
	    perror("error un-mmapping sensehat led matrix");
    }
    led_matrix_inited = 0;
    close(led_matrix_fd);
}

void led_matrix_set_val(int x, int y, uint16_t val){
    led_matrix[LED_MATRIX_WIDTH*y+x] = val;
}

void led_matrix_clear(){
    memset(led_matrix, 0, LED_MATRIX_FILESIZE);
}

void led_matrix_fill(uint16_t val){
    for(int i = 0; i < LED_MATRIX_NUM_LEDS; i++){
        led_matrix[i] = val;
    }
}

uint16_t rgb_int_to_hex(uint16_t r, uint16_t g, uint16_t b){

    r = r << 11;

    g = g << 5;

    return r|g|b;
}

uint16_t rgb_double_to_hex(double r, double g, double b){
    return rgb_int_to_hex((uint16_t)(r*31),(uint16_t)(g*63),(uint16_t)(b*31));
}

void blit_pixel(double x, double y, double r, double g, double b){
    double px = trunc(x);
    double py = trunc(y);
    int pxi = (int)px;
    int pyi = (int)py;


    double opacity = (1.0 + px - x)*(1.0 + py - y);
    led_matrix_set_val(pxi,pyi,rgb_double_to_hex(opacity*r,opacity*g,opacity*b));

    opacity = (x - px)*(py + 1.0 - y);
    led_matrix_set_val((pxi + 1),pyi,rgb_double_to_hex(opacity*r,opacity*g,opacity*b));

    opacity = (px + 1.0 - x)*(y - py);
    led_matrix_set_val(pxi, (pyi + 1),rgb_double_to_hex(opacity*r,opacity*g,opacity*b));

    opacity = (x-px)*(y-py);
    led_matrix_set_val((pxi + 1),(pyi+1),rgb_double_to_hex(opacity*r,opacity*g,opacity*b));
}

static int raster_line_generator(int *x, int *y, int endX, int endY){
    if(*x == endX && *y == endY){ return 0; }

    double diffX = (double)(endX - *x);
    double diffY = (double)(endY - *y);

    if(endX != *x){
        double slope = diffY/diffX;
        if(slope < -0.25 || slope > 0.25){
            if (diffY > 0){
                *y+=1;
            }
            else{
                *y-=1;
            }
        }

        if(slope > -4 && slope < 4){
            if (diffX > 0){
                *x+=1;
            }
            else{
                *x-=1;
            }
        }
        return 1;
    }

    if (diffY > 0){
        *y+=1;
    }
    else{
        *y-=1;
    }
    return 1;
}

void blit_colored_line(int startX, int startY, int endX, int endY, double r, double g, double b){
    led_matrix_set_val(startX,startY,rgb_double_to_hex(r,g,b));
    while(raster_line_generator(&startX, &startY, endX, endY)){
        led_matrix_set_val(startX, startY, rgb_double_to_hex(r,g,b));
    }
}
