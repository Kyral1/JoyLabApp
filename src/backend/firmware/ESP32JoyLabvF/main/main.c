#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "LED_Control.h"
#include "Button_Control.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Bluetooth.h"
#include "driver/gpio.h"

static const char *TAG = "MAIN";
#define BUTTON_GPIO 21


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

    /*led_init();

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0,
        .pull_down_en = 1,  // enable internal pulldown
        .intr_type = GPIO_INTR_DISABLE 
    };
    gpio_config(&io_conf);*/

    //int color_state = 0;
    //int num_leds = 72;

    while(1){
        /*int button_state = gpio_get_level(BUTTON_GPIO);
        printf("Button state: %d\n", button_state);

        if (button_state == 1)
        {
            led_clear();
            led_show();
        }
        else
        {
            if (color_state == 0){
                for (int i = 0; i < num_leds; i++){
                    led_set_color_brightness(i, 255, 0, 255, 0.5f);  // Red
                }
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            else if (color_state == 1)
            {
                for (int i = 0; i < num_leds; i++){
                    led_set_color_brightness(i, 0, 0, 255, 0.5f);  // Blue
                }
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            else if (color_state == 2) 
            {
                for (int i = 0; i < num_leds; i++){
                    led_set_color_brightness(i, 0, 255, 255, 0.5f);  // Green
                }
                vTaskDelay(pdMS_TO_TICKS(1000));
            }

            led_show();

            // Move to next color
            color_state = (color_state + 1) % 3;

        }*/

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

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

/*
//-----------LED TESTING -----------
void app_main(void) {
    led_init();  // initialize the LED strip

    // Turn all 72 LEDs red (R=255, G=0, B=0) at 70% brightness
    for (int i = 0; i < 72; i++) {
        led_set_color_brightness(i, 255, 0, 0, 0.5f);
    }

    led_show();  // Refresh to apply changes
    ESP_LOGI("MAIN", "All LEDs turned on to red!");

    // Example test sequence w/o BLE connection
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}*/


/*
//Button simulation
void app_main(void){
    led_init();

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = 0,
        .pull_down_en = 1,  // enable internal pulldown
        .intr_type = GPIO_INTR_DISABLE 
    };
    gpio_config(&io_conf);

    while(1){
        int button_state = gpio_get_level(BUTTON_GPIO);
        printf("Button state: %d\n", button_state);

        if (button_state == 1)
        {
            // Button HIGH → LEDs red
            for (int i = 0; i < 72; i++)
                led_set_color_brightness(i, 255, 0, 0, 0.5f);
            led_show();
        }
        else
        {
            // Button LOW → LEDs off
            led_clear();
            led_show();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}*/


/*
void app_main(void) {
    led_init();

    while (1) {
        //Turn all LEDs red
        for (int i = 0; i < 72; i++) {
            led_set_color_brightness(i, 255, 0, 0, 0.5f);
        }
        led_show();
        vTaskDelay(pdMS_TO_TICKS(1000));

        //Clear (off)
        led_clear();
        led_show();
        vTaskDelay(pdMS_TO_TICKS(500));

        //Turn all LEDs blue
        for (int i = 0; i < 72; i++) {
            led_set_color_brightness(i, 0, 0, 255, 0.5f);
        }
        led_show();
        vTaskDelay(pdMS_TO_TICKS(1000));

        //Clear again
        led_clear();
        led_show();
        vTaskDelay(pdMS_TO_TICKS(500));

        //Turn all LEDs green
        for (int i = 0; i < 72; i++) {
            led_set_color_brightness(i, 0, 255, 0, 0.5f);
        }
        led_show();
        vTaskDelay(pdMS_TO_TICKS(1000));

        //Final clear
        led_clear();
        led_show();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}*/