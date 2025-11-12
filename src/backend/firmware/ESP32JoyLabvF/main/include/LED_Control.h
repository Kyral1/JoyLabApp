#pragma once
#include <stdint.h>

typedef struct{
    uint8_t r,g,b;
    float brightness;
}ButtonColor;

void led_init(void);
void led_set_color_brightness(int index, uint8_t r, uint8_t g, uint8_t b, float brightness);
void led_clear_one(int index);
void led_clear(void);
void led_set_global_brightness(float brightness);
void led_show(void);
void set_button_color(int button_num, uint8_t r, uint8_t g, uint8_t b, float brightness);
void save_button_state(int button, uint8_t r, uint8_t g, uint8_t b, float brightness);
void save_button_color(int button, uint8_t r, uint8_t g, uint8_t b, float brightness);
void load_button_states_from_nvs(void);
void save_button_state_persistent(int button, uint8_t r, uint8_t g, uint8_t b, float brightness);
ButtonColor get_button_state(int button);
ButtonColor get_button_color(int button);
