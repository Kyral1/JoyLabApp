#pragma once
#include <stdint.h>

void led_init(void);
void led_set_color_brightness(int index, uint8_t r, uint8_t g, uint8_t b, float brightness);
void led_clear_one(int index);
void led_clear(void);
void led_set_global_brightness(float brightness);
void led_show(void);
