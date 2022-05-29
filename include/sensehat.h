#ifndef SENSEHAT_H_
#define SENSEHAT_H_

#include <stdint.h>

#define LED_MATRIX_NUM_LEDS 64
#define LED_MATRIX_FILESIZE (LED_MATRIX_NUM_LEDS * sizeof(uint16_t))

int init_sensehat_led_matrix();

void destroy_sensehat_led_matrix();

void led_matrix_set_val(int x, int y, uint16_t val);

void led_matrix_clear();

void led_matrix_fill(uint16_t val);

// 16 bit color conversions
uint16_t rgb_int_to_hex(uint16_t r, uint16_t g, uint16_t b);

uint16_t rgb_double_to_hex(double r, double g, double b);

void blit_pixel(double x, double y, double r, double g, double b);

void blit_colored_line(int startX, int startY, int endX, int endY, double r, double g, double b);

#endif

