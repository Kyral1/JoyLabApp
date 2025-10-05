#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "bluetooth.h"

static const char *TAG = "MAIN";

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
}
