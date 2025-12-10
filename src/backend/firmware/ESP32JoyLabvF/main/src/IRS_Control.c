#include "IRS_Control.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "esp_log.h"
#include "vl53l0x.h"
#include "driver/gpio.h"
#include "Bluetooth.h"

static const char *TAG = "IRS_CONTROL";
static TaskHandle_t irs_task_handle = NULL;
static bool IRS_running = false;
static VL53L0X_Dev_t sensor;

#define PROXIMITY_THRESHOLD_MM 100  // Adjust this for sensitivity (e.g., 50–150mm)

void irs_init(void) {
    gpio_set_direction(14, GPIO_MODE_OUTPUT);
    gpio_set_level(14, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(14, 1);
    vTaskDelay(pdMS_TO_TICKS(50));
    ESP_LOGW(TAG, "IRS INIT CALLED");
    ESP_LOGI(TAG, "Initializing VL53L0X (ESP32 driver)...");
    sensor.I2cDevAddr = 0x29;
    VL53L0X_Error err = VL53L0X_Device_init(&sensor);
    if (err != VL53L0X_ERROR_NONE)
    {
        ESP_LOGE(TAG, "VL53L0X init failed (%d)", err);
        return;
    }

    ESP_LOGI(TAG, "VL53L0X ready!");

}

uint16_t irs_read_distance_mm(void) {
    ESP_LOGI(TAG, "Initialized");
    uint16_t dist = 0;
    if (VL53L0X_Device_getMeasurement(&sensor, &dist) == VL53L0X_ERROR_NONE)
    {
        ESP_LOGI(TAG, "Distance: %d mm", dist);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to read distance");
    }
    return dist;
}

bool irs_is_hand_near(void) {
    VL53L0X_RangingMeasurementData_t measure;
    VL53L0X_GetRangingMeasurementData(&sensor, &measure);

    if (measure.RangeStatus == 0 && measure.RangeMilliMeter < PROXIMITY_THRESHOLD_MM) {
        VL53L0X_ClearInterruptMask(&sensor, 0);
        return true;
    }
    VL53L0X_ClearInterruptMask(&sensor, 0);
    return false;
}

void irs_continuous_task(void *pvParameters) {
    ensure_irs_ready(); 
    IRS_running = true; 
    while (IRS_running) {
        uint16_t distance = irs_read_distance_mm();
        ble_notify_distance(distance);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    irs_task_handle = NULL;
    vTaskDelete(NULL);
}


void start_irs_continuous_task(void) {
    if (irs_task_handle == NULL) {
        ESP_LOGI(TAG, "Starting IRS continuous updates...");
        xTaskCreate(
            irs_continuous_task,    // Task function
            "irs_continuous_task",  // Task name
            4096,                   // Stack size
            NULL,                   // Parameters
            5,                      // Priority
            &irs_task_handle        // Task handle
        );
    } else {
        ESP_LOGW(TAG, "IRS continuous task already running");
    }
}

void stop_irs_continuous_task(void) {
    if (irs_task_handle != NULL) {
        ESP_LOGI(TAG, "Stopping IRS continuous updates...");
        IRS_running = false; 
    } else {
        ESP_LOGW(TAG, "IRS continuous task not running");
    }
    irs_task_handle = NULL;
}
