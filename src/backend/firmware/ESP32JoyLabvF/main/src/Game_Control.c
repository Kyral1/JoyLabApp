#include "Game_Control.h"
#include "LED_Control.h"
#include "Button_Control.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"
#include "Bluetooth.h"
#include "IRS_Control.h"

static TaskHandle_t game_task_handle = NULL;
static bool game_running = false;
static const char *TAG = "GAME_CONTROL";
static int led_mode_points = 0;
static int led_mode_attempts = 0;

static void whackamole_game_task(void *pvParameters) {
    ensure_led_ready();   // from bluetooth.c
    button_init_all();    // from Button_Control.c
    load_button_states_from_nvs(); 
    led_clear();
    //irs_init();


    ESP_LOGI(TAG, "Starting Whack-A-Mole game!");
    game_running = true;

    bool attempt_logged = false;

    while (game_running) {
        // Choose a random button 0–3
        int target = esp_random() % NUM_BUTTONS;

        //get button state
        ButtonColor color = get_button_color(target);
        ESP_LOGI(TAG, "Target %d -> r=%d g=%d b=%d br=%.2f",target, color.r, color.g, color.b, color.brightness);

        //light up button:
        for (int i = 0; i<NUM_BUTTONS;i++){
            if(i==target){
                set_button_color(target, color.r, color.g, color.b, color.brightness);
            }else{
                set_button_color(i, 0, 0, 0, 0);
            }
        }
        led_show();
        ESP_LOGI(TAG, "after LED show");

        // Wait for correct button press
        while (game_running) {
            //bool near = irs_is_hand_near();  
            bool pressed = button_is_pressed(target);
            //ESP_LOGI(TAG, "inside while loop");

            /*if (near && !attempt_logged) {
                led_mode_attempts ++;
                evt_notify_led_whack_result(led_mode_points, led_mode_attempts);
                attempt_logged = true;
                ESP_LOGI(TAG, "Attempt detected for button %d", target);
            }*/
            if(pressed){
                ESP_LOGI(TAG, "in pressed loop");
                led_mode_points ++;
                evt_notify_led_whack_result(led_mode_points, led_mode_attempts);
                ESP_LOGI(TAG, "Button %d pressed! (+1 point)", target);
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
    // Set handle to NULL before delete
    game_task_handle = NULL;
    vTaskDelete(NULL);
}

// ================= PUBLIC FUNCTIONS =================

void start_whackamole_game(void) {
    if (game_task_handle == NULL) {
        led_mode_points = 0;
        led_mode_attempts = 0;
        evt_notify_led_whack_result(led_mode_points, led_mode_attempts);

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

/*void stop_whackamole_game(void) {
    if (game_task_handle != NULL) {
        ESP_LOGI(TAG, "Stopping Whack-A-Mole...");
        game_running = false;
        vTaskDelete(game_task_handle);
        game_task_handle = NULL;
    }*/
void stop_whackamole_game(void) {
    if (game_task_handle != NULL) {
        ESP_LOGI(TAG, "Stopping Whack-A-Mole...");
        game_running = false;   // signal to end loop
    }
}
