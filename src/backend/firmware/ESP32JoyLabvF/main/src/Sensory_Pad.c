#include "VibrationMotor.h"
#include "ForceSensor_Control.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Bluetooth.h"

static const char *TAG = "SENSORY_PAD";

static TaskHandle_t sensory_task_handle = NULL;
static bool sensory_running = false;

static uint8_t sensory_mode = CMD_SENSORY_NO_VIBRATION;
static uint8_t const_intensity = 0;
static uint8_t inc_min_intensity = 0;
static uint8_t inc_max_intensity = 100;


// ================= SENSORY TASK =================

static void sensory_task(void *pvParameters) {
    force_sensor_init();
    vibration_init();

    sensory_running = true;
    ESP_LOGI(TAG, "Sensory task started (mode=%d)", sensory_mode);

    while (sensory_running) {
        float force = force_sensor_read();   // 0 – 25 approx
        uint8_t intensity = 0;

        switch (sensory_mode) {

        case CMD_SENSORY_NO_VIBRATION:
            vibration_set_state_motor1(false);
            vibration_set_state_motor2(false);
            break;

        case CMD_SENSORY_CONSTANT_VIBRATION:
            if (force > 0.3f) {
                vibration_set_state_motor1(true);
                vibration_set_state_motor2(true);

                vibration_set_intensity_motor1(constant_intensity);
                vibration_set_intensity_motor2(constant_intensity);
            } else {
                vibration_set_state_motor1(false);
                vibration_set_state_motor2(false);
            }
            break;

        case CMD_SENSORY_INCREASING_VIBRATION:
            if (force <= 0.3f) {
                vibration_set_state_motor1(false);
                vibration_set_state_motor2(false);
            } else {
                vibration_set_state_motor1(true);
                vibration_set_state_motor2(true);

                // map 0–25 → 0–100%
                intensity = inc_min_intensity + (uint8_t)((inc_max_intensity - inc_min_intensity) * percent);

                vibration_set_intensity_motor1(intensity);
                vibration_set_intensity_motor2(intensity);
            }
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
    vTaskDelete(sensory_task_handle);
    sensory_task_handle = NULL;
}

void sensory_stop(void) {
    if (sensory_task_handle != NULL) {
        sensory_running = false;
        vibration_set_state_motor1(false);
        vibration_set_state_motor2(false);
        ESP_LOGI(TAG, "Sensory mode stopped");
    }
}

//sensory no vibratio start
void sensory_no_vibration_start(void) {
    sensory_stop();
    if(sensory_task_handle == NULL){
        sensory_mode = CMD_SENSORY_NO_VIBRATION;
        xTaskCreate(sensory_task, "sensory_task", 4096, NULL, 5, &sensory_task_handle);
        ESP_LOGI(TAG, "Sensory no vibration started");
    }else{
        ESP_LOGW(TAG, "Sensory task already running (!NULL)");
    }
}

//sensory constant vibratio start
void sensory_constant_vibration_start(uint8_t intensity) {
    sensory_stop();
    if(sensory_task_handle == NULL){
        sensory_mode = CMD_SENSORY_CONSTANT_VIBRATION;
        const_intensity = intensity;
        if (const_intensity > 100) const_intensity = 100;
        xTaskCreate(sensory_task, "sensory_task", 4096, NULL, 5, &sensory_task_handle);
        ESP_LOGI(TAG, "Sensory constant vibration started");
    }else{
        ESP_LOGW(TAG, "Sensory task already running (!NULL)");
    }
}

//sensory increasing vibratio start
void sensory_increasing_vibration_start(uint8_t min_int, uint8_t max_int) {
    sensory_stop();
    if(sensory_task_handle == NULL){
        sensory_mode = CMD_SENSORY_INCREASING_VIBRATION;
        inc_min_intensity = min_int;
        inc_max_intensity = max_int;
        if (inc_min_intensity > 100) inc_min_intensity = 100;
        if (inc_max_intensity > 100) inc_max_intensity = 100;
        if (inc_min_intensity > inc_max_intensity) inc_min_intensity = inc_max_intensity;
        xTaskCreate(sensory_task, "sensory_task", 4096, NULL, 5, &sensory_task_handle);
    }else{
        ESP_LOGW(TAG, "Sensory task already running (!NULL)");
    }
}