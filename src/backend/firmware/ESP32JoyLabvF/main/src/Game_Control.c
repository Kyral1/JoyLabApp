#include "Game_Control.h"
#include "LED_Control.h"
#include "Button_Control.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//LEDs to Button Config
static const int led_start_for_button[NUM_BUTTONS] = {0, 18, 36, 54};  // Adjust once mapping decided
static const int leds_per_button = 18;

static TaskHandle_t game_task_handle = NULL;
static bool game_running = false;

static void whackamole_game_task(void *pvParameters) {
    ensure_led_ready();   // from LED_Control.c
    button_init_all();    // from Button_Control.c

    ESP_LOGI(TAG, "Starting Whack-A-Mole game!");
    game_running = true;

    while (game_running) {
        // Choose a random button 0–3
        int target = esp_random() % NUM_BUTTONS;

        // Turn on LEDs for that button
        for (int i = 0; i < leds_per_button; i++) {
            int led_idx = led_start_for_button[target] + i;
            led_set_color_brightness(led_idx, 255, 0, 0, 0.5f); // red, 50% brightness
        }
        led_show();

        // Wait for correct button press
        while (game_running) {
            if (button_is_pressed(target)) {
                ESP_LOGI(TAG, "Button %d hit!", target);
                // Turn off LEDs for that button
                for (int i = 0; i < leds_per_button; i++) {
                    led_clear_one(led_start_for_button[target] + i);
                }
                led_show();
                vTaskDelay(pdMS_TO_TICKS(250)); // small pause
                break; // move to next round
            }
            vTaskDelay(pdMS_TO_TICKS(50)); // polling interval
        }
    }

    // Cleanup when game stops
    led_clear_all();
    led_show();
    ESP_LOGI(TAG, "Whack-A-Mole stopped.");
    vTaskDelete(NULL);
}

// ================= PUBLIC FUNCTIONS =================

void start_whackamole_game(void) {
    if (game_task_handle == NULL) {
        xTaskCreate(
            whackamole_game_task,
            "whackamole_game_task",
            4096,
            NULL,
            5,
            &game_task_handle
        );
    } else {
        ESP_LOGW(TAG, "Whack-A-Mole already running.");
    }
}

void stop_whackamole_game(void) {
    if (game_task_handle != NULL) {
        ESP_LOGI(TAG, "Stopping Whack-A-Mole...");
        game_running = false;
        vTaskDelete(game_task_handle);
        game_task_handle = NULL;
    }
}