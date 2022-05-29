#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define GPIO_DIRECTION_FMT "/sys/class/gpio/gpio%s/direction"
#define GPIO_VALUE_FMT "/sys/class/gpio/gpio%s/value"

#define MAX_PIN_STR_SIZE 12
static char pin_num_buff[MAX_PIN_STR_SIZE];

static int _export_pin(int export_fd, int pin_num_len, char *direction, int direction_len){
    int fd;

    char *gpio_direction_str = malloc((strlen(GPIO_DIRECTION_FMT) - 2 + pin_num_len + 1) * sizeof(char));
    sprintf(gpio_direction_str, GPIO_DIRECTION_FMT, pin_num_buff);

    char *gpio_value_str = malloc((strlen(GPIO_VALUE_FMT) - 2 + pin_num_len + 1) * sizeof(char));
    sprintf(gpio_value_str, GPIO_VALUE_FMT, pin_num_buff);

    if(access(gpio_value_str, F_OK)){
        if(write(export_fd, pin_num_buff, pin_num_len) != pin_num_len){
            printf("Error writing to /sys/class/gpio/export\n");
            perror("");
            free(gpio_direction_str);
            free(gpio_value_str);
            return -1;
        };
    }

    fd = open(gpio_direction_str, O_WRONLY);
    if (fd == -1) {
        printf("Unable to open %s\n", gpio_direction_str);
        perror("");
        free(gpio_direction_str);
        free(gpio_value_str);
        return -1;
    }

    if (write(fd, direction, direction_len) != direction_len) {
        printf("Error writing to %s\n", gpio_direction_str);
        perror("");
        free(gpio_direction_str);
        free(gpio_value_str);
        return -1;
    }

    close(fd);

    fd = open(gpio_value_str, O_WRONLY);
    if (fd == -1) {
        printf("Unable to open %s\n", gpio_value_str);
        perror("");
        free(gpio_direction_str);
        free(gpio_value_str);
        return -1;
    }

    free(gpio_direction_str);
    free(gpio_value_str);

    return fd;
}


int export_pin(int pin_num, char *direction){
    int direction_len = strlen(direction);

    int export_fd = open("/sys/class/gpio/export", O_WRONLY);
    if (export_fd == -1) {
        printf("Unable to open /sys/class/gpio/export\n");
        perror("");
        return -1;
    }

    sprintf(pin_num_buff ,"%i", pin_num);
    int pin_num_len = strlen(pin_num_buff);

    int fd = _export_pin(export_fd, pin_num_len, direction, direction_len);
    close(export_fd);
    return fd;
}

int export_pins(int start_pin, int num_pins, int* fds, char *direction){
    for(int i = 0; i < num_pins; i++){
        fds[i] = -1;
    }

    int export_fd = open("/sys/class/gpio/export", O_WRONLY);
    if (export_fd == -1) {
        printf("Unable to open /sys/class/gpio/export\n");
        perror("");
        return 0;
    }

    int pin_num;
    int pin_num_len;
    int direction_len = strlen(direction);

    for(int i=0; i < num_pins; i++){
        pin_num = i + start_pin;
        sprintf(pin_num_buff ,"%i", pin_num);
        pin_num_len = strlen(pin_num_buff);
        fds[i] = _export_pin(export_fd, pin_num_len, direction, direction_len);
        if(fds[i] < 0){
            return 0;
        }
    }

    close(export_fd);
    return 1;
}

int unexport_pin(int pin_num, int pin_fd){

    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
        printf("Unable to open /sys/class/gpio/unexport\n");
        perror("");
        return 0;
    }

    sprintf(pin_num_buff ,"%i", pin_num);
    int pin_num_len = strlen(pin_num_buff);

    if (write(fd, pin_num_buff, pin_num_len) != pin_num_len) {
        printf("Error writing to /sys/class/gpio/unexport\n");
        perror("");
        return 0;
    }

    close(fd);
    close(pin_fd);
    return 1;
}

int unexport_pins(int start_pin, int num_pins, int* fds){
    int unexport_fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (unexport_fd == -1) {
        printf("Unable to open /sys/class/gpio/unexport\n");
        perror("");
        return 0;
    }

    int pin_num;
    int pin_num_len;
    for(int i=0; i < num_pins; i++){
        if(fds[i] < 0){continue;}
        pin_num = i + start_pin;
        sprintf(pin_num_buff ,"%i", pin_num);
        pin_num_len = strlen(pin_num_buff);

        if (write(unexport_fd, pin_num_buff, pin_num_len) != pin_num_len) {
            printf("Error writing to /sys/class/gpio/unexport\n");
            perror("");
            continue;
            //return 0;
        }

        close(fds[i]);
        fds[i] = -1;
    }

    close(unexport_fd);
    return 1;
}
