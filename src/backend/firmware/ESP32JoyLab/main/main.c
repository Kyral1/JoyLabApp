#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "bluetooth.h"
#include "LED_Control.h"
#include "Button_Control.h"

static const char *TAG = "MAIN";

//-----------BLE TESTING -----------
/*void app_main(void)
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
}*/

//-----------BUTTON + LED TESTING -----------
/*
void app_main(void) {
    led_init();
    button_init_all();

    while (1) {
        for (int i = 0; i < NUM_BUTTONS; i++) {
            if (button_is_pressed(i)) {
                ESP_LOGI(TAG, "Button %d pressed", i);
                led_set_color_brightness(i, 255, 0, 0, 1.0f);  // LED on
            } else {
                led_clear_one(i);  // LED off
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));  // smooth polling
    }
}*/


//-----------LED TESTING -----------
void app_main(void) {
    led_init();  // initialize the LED strip

    // Example test sequence w/o BLE connection
    while (1) {
        // LED 0 Red, full brightness
        led_set_color_brightness(0, 255, 0, 0, 1.0f);
        vTaskDelay(pdMS_TO_TICKS(1000));

        // LED 0 Green, half brightness
        led_set_color_brightness(0, 0, 255, 0, 0.5f);
        vTaskDelay(pdMS_TO_TICKS(1000));

        // LED 0 Blue, 25% brightness
        led_set_color_brightness(0, 0, 0, 255, 0.25f);
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Turn all LEDs off
        led_clear();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}