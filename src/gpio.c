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

int export_pin(char *pin_num, char *direction){
    int pin_num_len = strlen(pin_num);
    int direction_len = strlen(direction);

    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        printf("Unable to open /sys/class/gpio/export\n");
        perror("");
        return -1;
    }

    write(fd, pin_num, pin_num_len);
    close(fd);

    char *gpio_direction_str = malloc((strlen(GPIO_DIRECTION_FMT) - 2 + pin_num_len + 1) * sizeof(char));
    sprintf(gpio_direction_str, GPIO_DIRECTION_FMT, pin_num);

    char *gpio_value_str = malloc((strlen(GPIO_VALUE_FMT) - 2 + pin_num_len + 1) * sizeof(char));
    sprintf(gpio_value_str, GPIO_VALUE_FMT, pin_num);

    fd = open(gpio_direction_str, O_WRONLY);
    if (fd == -1) {
        printf("Unable to open %s\n", gpio_direction_str);
        perror("");
        free(gpio_direction_str);
        free(gpio_value_str);
        exit(1);
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

int unexport_pin(char *pin_num, int pin_fd){
    int pin_num_len = strlen(pin_num);

    int fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (fd == -1) {
        printf("Unable to open /sys/class/gpio/unexport\n");
        perror("");
        return 0;
    }

    if (write(fd, pin_num, pin_num_len) != pin_num_len) {
        printf("Error writing to /sys/class/gpio/unexport\n");
        perror("");
        return 0;
    }

    close(fd);
    close(pin_fd);
    return 1;
}
