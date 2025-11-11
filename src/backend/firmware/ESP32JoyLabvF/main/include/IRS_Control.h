#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c.h"
#include "esp_err.h"

// Initializes the VL53L0X infrared distance sensor
void irs_init(void);

// Reads the current distance measurement in millimeters
uint16_t irs_read_distance_mm(void);
bool irs_is_hand_near(void);
