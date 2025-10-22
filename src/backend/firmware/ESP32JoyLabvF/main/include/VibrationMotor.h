#pragma once
#include <stdbool.h>
#include "driver/gpio.h"

#define VIBRATION_GPIO 19

//functions
void vibration_init(void);
void vibration_set_state(bool on);
bool vibration_get_state(void);
