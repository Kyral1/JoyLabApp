#pragma once
#include <stdbool.h>
#include "driver/gpio.h"

void vibration_init(void);

// Motor 1 controls
void vibration_set_state_motor1(bool on);
void vibration_set_intensity_motor1(uint8_t intensity);

// Motor 2 controls
void vibration_set_state_motor2(bool on);
void vibration_set_intensity_motor2(uint8_t intensity);
