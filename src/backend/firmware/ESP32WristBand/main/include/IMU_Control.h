#pragma once
#include <stdbool.h>
#include "bmi323.h"

extern struct bmi3_dev imu_dev;

void imu_init(void);

bool imu_motion_detected(void);

bool imu_read_accel(float *x, float *y, float *z);

int imu_get_orientation(void);

/*#pragma once
#include <stdint.h>
#include <stdbool.h>
extern struct bmi3_dev imu_dev;
void imu_init(void);
bool imu_motion_detected(void);
int imu_get_orientation(void); 
void imu_poll(void);*/
