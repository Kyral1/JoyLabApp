#pragma once

#include <stdbool.h>

static void no_vibration(void);

static void constant_vibration(uint8_t intensity);

static void increasing_vibration(uint8_t start_intensity, uint8_t end_intensity);