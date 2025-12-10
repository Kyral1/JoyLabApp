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

static const char *TAG = "MAIN WB";
//-----------BLE TESTING -----------
void app_main(void)
{
    // Initialize NVS (required for BLE stack)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize Bluetooth — starts advertising automatically
    ESP_ERROR_CHECK(bluetooth_init());
    ESP_LOGI(TAG, "Bluetooth initialized and advertising started");
    vTaskDelay(pdMS_TO_TICKS(1000)); 

    ESP_LOGI(TAG, "Initializing IMU...");
    imu_init();
    vTaskDelay(pdMS_TO_TICKS(300));

    float x, y, z;

    while (1)
    {
        if (imu_motion_detected()) {
            ESP_LOGW("MAIN", "ANY-MOTION DETECTED!");
        }

        if (imu_read_accel(&x, &y, &z)) {
            ESP_LOGI("MAIN", "ACCEL  X=%.2f  Y=%.2f  Z=%.2f", x, y, z);
        }

        int o = imu_get_orientation();
        ESP_LOGI("MAIN", "Orientation = %d", o);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
