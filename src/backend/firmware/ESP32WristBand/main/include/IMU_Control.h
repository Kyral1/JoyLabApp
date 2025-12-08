#pragma once
#include <stdint.h>
#include <stdbool.h>

void imu_init(void);
bool imu_motion_detected(void);
int imu_get_orientation(void); 
void imu_poll(void);