#pragma once

#include <stdbool.h>


void sensory_stop(void);
//sensory no vibratio start
void sensory_no_vibration_start(void);
//sensory constant vibratio start
void sensory_constant_vibration_start(uint8_t intensity);
//sensory increasing vibratio start
void sensory_increasing_vibration_start(uint8_t min_int, uint8_t max_int);