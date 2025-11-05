#include "Game_Control.h"
#include "LED_Control.h"
#include "Button_Control.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"
#include "Bluetooth.h"

static TaskHandle_t game_task_handle = NULL;
static bool game_running = false;
static const char *TAG = "GAME_CONTROL";

static void whackamole_game_task(void *pvParameters) {
    ensure_led_ready();   // from bluetooth.c
    button_init_all();    // from Button_Control.c

    ESP_LOGI(TAG, "Starting Whack-A-Mole game!");
    game_running = true;

    while (game_running) {
        // Choose a random button 0–3
        int target = esp_random() % NUM_BUTTONS;

        //get button state
        ButtonColor state = get_button_state(target);

        //light up button:
        set_button_color(target + 1, state.r, state.g, state.b, state.brightness);
        led_show();

        // Wait for correct button press
        while (game_running) {
            if (button_is_pressed(target)) {
                ESP_LOGI(TAG, "Button %d hit!", target);
                // Turn off LEDs for that button
                set_button_color(target, 0, 0, 0, 0.0f);
                led_show();
                vTaskDelay(pdMS_TO_TICKS(250)); // small pause
                break; // move to next round
            }
            vTaskDelay(pdMS_TO_TICKS(50)); // polling interval
        }
    }

    // Cleanup when game stops
    led_clear();
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