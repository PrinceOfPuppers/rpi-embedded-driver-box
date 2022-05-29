#ifndef GPIO_H_
#define GPIO_H_

// returns fd
int export_pin(int pin_num, char *direction);

// returns 0 => fail, 1 => success
int unexport_pin(int pin_num, int pin_fd);

// returns 0 => fail, 1 => success
int unexport_pins(int start_range, int num_pins, int* fds);

// returns 0 => fail, 1 => success
int export_pins(int start_range, int num_pins, int* fds, char *direction);

#endif
