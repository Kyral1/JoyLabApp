#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c.h"
#include "esp_err.h"

void force_sensor_init(void);
float force_sensor_read(void);