#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "LED_Control.h"
#include "Button_Control.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Bluetooth.h"
#include "driver/gpio.h"
#include "VibrationMotor.h"
#include "Speaker_Control.h"
#include "buttonSound.h"
#include "ForceSensor_Control.h"
#include "vibrationMotor.h"
#include "esp_spiffs.h"
#include "IRS_Control.h"
#include "Sensory_Pad.h"
#include "Game_Control.h"

static const char *TAG = "MAIN";
#define BUTTON_GPIO 21

void filesystem_init(void)
{
    ESP_LOGI("SPIFFS", "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "spiffs",
        .max_files = 5,
        .format_if_mount_failed = true
    };

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    size_t total = 0, used = 0;
    esp_spiffs_info("spiffs", &total, &used);
    ESP_LOGI("SPIFFS", "Partition size: total: %d bytes, used: %d bytes", total, used);
}

//-----------BLE TESTING -----------
void app_main(void)
{
    //irs_init();
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
    filesystem_init();
    vTaskDelay(pdMS_TO_TICKS(1000));
    speaker_init();
    //speaker_set_volume(70);
    //speaker_play_wav("HappyNoise3.wav");
    //ensure_irs_ready();
    //force_sensor_init();
    //speaker_play_mp3_file("/spiffs/test.mp3");
    
    //load_button_sounds_from_nvs();
    load_button_states_from_nvs(); 
    //start_whackamole_game();
    //start_led_reg_game();
    /*vTaskDelay(pdMS_TO_TICKS(1000));
    ensure_led_ready();
    vTaskDelay(pdMS_TO_TICKS(1000));
    led_clear();
    vTaskDelay(pdMS_TO_TICKS(1000));
    led_show();
    vTaskDelay(pdMS_TO_TICKS(1000));
    set_button_color(1, 70, 0, 0, 1.0f);
    vTaskDelay(pdMS_TO_TICKS(1000));
    led_show();*/
    //vTaskDelay(pdMS_TO_TICKS(1000));
    //vibration_init();
    //vTaskDelay(pdMS_TO_TICKS(3000));
    //vibration_set_state_motor2(false);

    /*ensure_force_ready();
    vibration_init();
    vibration_set_state_motor1(true);
    vibration_set_intensity_motor1(75);
    vTaskDelay(pdMS_TO_TICKS(1000));
    vibration_set_state_motor1(false);*/
    //sensory_constant_vibration_start(60);
    //sensory_increasing_vibration_start(20, 100);
    /*speaker_init();
    speaker_set_volume(70);
    speaker_beep_blocking(1000, 500);
    speaker_set_volume(100);
    speaker_play_wav("/spiffs/HappyNoise.wav");

    

    ESP_LOGI("TEST", "Opening test file...");

    FILE *f = fopen("/spiffs/HappyNoise.wav", "r");
    if (!f) {
        ESP_LOGE("TEST", "Failed to open wav");
    } else {
        ESP_LOGI("TEST", "SUCCESS! wav found 🎉");
        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        ESP_LOGI("MP3", "MP3 file size: %ld bytes", size);
        fclose(f);
    }*/

    //speaker_play_mp3_stream_safe("/spiffs/test.mp3");

    //xTaskCreate(mp3_test_task, "mp3_test_task", 4096, NULL, 5, NULL);
    //speaker_play_mp3_file("/spiffs/test.mp3");
    //speaker_play_mp3_stream_safe("/spiffs/test.mp3");
    //vTaskDelay(pdMS_TO_TICKS(1000));

    //while (1) {
        /*ensure_speaker_ready();
        vTaskDelay(pdMS_TO_TICKS(1000));
        speaker_set_volume(70);
        speaker_beep_blocking(1000, 500);

        vTaskDelay(pdMS_TO_TICKS(1000));

        speaker_set_volume(100);
        speaker_beep_blocking(1000, 500);*/

        /*ensure_irs_ready();
        uint16_t distance = irs_read_distance_mm();
        ESP_LOGI(TAG, "IRS Distance: %d mm", distance);
        vTaskDelay(pdMS_TO_TICKS(2000));*/

        /*float pressure = force_sensor_read();
        ESP_LOGI(TAG, "Force Sensor Pressure: %.2f N", pressure);
        vTaskDelay(pdMS_TO_TICKS(100));*/
        //vTaskDelay(pdMS_TO_TICKS(1000));
    //}

    while(1){
        vibration_init();
        vibration_set_state_motor1(true);
        vibration_set_intensity_motor1(75);
        //vibration_set_state_motor1(true);
        //vibration_set_intensity_motor1(75);
        //vTaskDelay(pdMS_TO_TICKS(2000));
        //vibration_set_state_motor2(false);
        //vibration_set_intensity_motor2(50);
        /*ensure_irs_ready();
        uint16_t distance = irs_read_distance_mm();
        ESP_LOGI(TAG, "IRS Distance: %d mm", distance);*/
        
        /*ESP_LOGI(TAG, "START--------------------------------------------------------");
        bool pressed0 = button_is_pressed(0);
        ESP_LOGI(TAG, "Button 0 pressed state: %d", pressed0);
        vTaskDelay(pdMS_TO_TICKS(250));
        bool pressed1 = button_is_pressed(1);
        ESP_LOGI(TAG, "Button 1 pressed state: %d", pressed1);
        vTaskDelay(pdMS_TO_TICKS(250));
        bool pressed2 = button_is_pressed(2);
        ESP_LOGI(TAG, "Button 3 pressed state: %d", pressed2);
        vTaskDelay(pdMS_TO_TICKS(250));
        bool pressed3 = button_is_pressed(3);
        ESP_LOGI(TAG, "Button 4 pressed state: %d", pressed3);
        vTaskDelay(pdMS_TO_TICKS(250));
        ESP_LOGI(TAG, "END--------------------------------------------------------");*/

        //ensure_speaker_ready();
        //speaker_set_volume(100);
        //speaker_play_wav("/spiffs/HappyNoise.wav");
        //speaker_beep_blocking(1000, 500);
        //vTaskDelay(pdMS_TO_TICKS(1000));
        //speaker_set_volume(10);
        //speaker_beep_blocking(1000, 500);
        //speaker_play_wav("/spiffs/HappyNoise.wav");
        //speaker_set_volume(100);
        //speaker_beep_blocking(1000, 500);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    //force_sensor_init();

    /*while(1){
        float pressure = force_sensor_read();
        ESP_LOGI(TAG, "Force Sensor Pressure: %.2f N", pressure);
        //vTaskDelay(pdMS_TO_TICKS(1000));
    }*/

    /*while (1){
        speaker_init();
        speaker_set_volume(70);
        speaker_beep_blocking(1000, 500);

        vTaskDelay(pdMS_TO_TICKS(1000));

        speaker_set_volume(100);
        speaker_beep_blocking(1000, 500);
    }*/

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

    /*while(1){
        int button_state = gpio_get_level(BUTTON_GPIO);
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

        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }*/
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