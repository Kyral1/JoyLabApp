#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Bluetooth.h"
#include "driver/gpio.h"
#include "IMU_Control.h"
#include "bmi323.h"
#include "bmi323_defs.h"
#include "driver/i2c.h"
#include "esp_rom_sys.h"
#include "math.h"
#include "bluetooth.h"
#include "Motion_task.h"

static float prev_x = 0;
static float prev_y = 0;
static float prev_z = 0;
static bool first_sample = true;
static uint32_t last_motion_time = 0;
float x, y, z;

static TaskHandle_t motion_task_handle = NULL;
static bool motion_running = false;
static const char *TAG = "MOTION_TASK";
static uint8_t motion_flags = 0;

static void motion_detection_task(void *pvParameters){
    ensure_BMI_ready();
    motion_running = true;
    while(motion_running){
        if (imu_read_accel(&x, &y, &z)) {

            //ESP_LOGI("MAIN", "ACCEL X=%.2f Y=%.2f Z=%.2f", x, y, z);

            // Skip the first sample (no previous reference)
                if (!first_sample) {

                    float dx = fabs(x - prev_x);
                    float dy = fabs(y - prev_y);
                    float dz = fabs(z - prev_z);

                    // threshold you want
                    const float THRESH = 400.0f;

                    uint32_t now = esp_log_timestamp();

                    const uint32_t MOTION_COOLDOWN = 500;

                    if (dx >= THRESH || dy >= THRESH || dz >= THRESH) {
                        if (now - last_motion_time > MOTION_COOLDOWN) {
                            ESP_LOGW("MAIN", "MOTION DETECTED! ΔX=%.1f  ΔY=%.1f  ΔZ=%.1f", dx, dy, dz);
                            motion_flags ++;
                            evt_notify_motion_detection(motion_flags);
                            last_motion_time = now; 
                        }
                    }
                }

            // Save current as previous
            prev_x = x;
            prev_y = y;
            prev_z = z;
            first_sample = false;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    motion_task_handle = NULL;
    vTaskDelete(NULL);
}


void start_motion_detection(void){
    if(motion_task_handle == NULL){
        ESP_LOGI(TAG, "Starting motion detection...");
        motion_flags = 0;
        evt_notify_motion_detection(motion_flags);
        xTaskCreate(
            motion_detection_task,
            "motion_detection_task",
            4096,
            NULL,
            5,
            &motion_task_handle
        );
    }
}

void stop_motion_detection(void) {
    if (motion_task_handle != NULL) {
        ESP_LOGI(TAG, "Stopping motion detection...");
        ESP_LOGI(TAG, "Final motion flags: %d", motion_flags);
        motion_running = false;   // signal to end loop
    }
    motion_task_handle = NULL;
}


