#pragma once
#include <stdbool.h>
#include "driver/gpio.h"

#define VIBRATION_01_GPIO 16
#define VIBRATION_02_GPIO 17

//functions
void vibration_init(gpio_num_t pin);
void vibration_set_state(bool on, gpio_num_t pin);
bool vibration_get_state(void);
