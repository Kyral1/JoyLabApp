#include "Game_Control.h"
#include "LED_Control.h"
#include "Button_Control.h"
#include "esp_log.h"
#include "buttonSound.h"
#include "speaker_control.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"
#include "Bluetooth.h"
#include "IRS_Control.h"
#include "bluetooth.h"

static TaskHandle_t game_task_handle = NULL;
static bool whack_game_running = false;
static const char *TAG = "GAME_CONTROL";
static int led_mode_points = 0;
static int led_mode_attempts = 0;

static bool led_reg_game_running = false;
static bool button_led_on[NUM_BUTTONS] = {false};
static int led_reg_interactions = 0;

static bool sound_reg_game_running = false;
static int sound_reg_points = 0;

static void sound_game_task(void *pvParameters) {
    ensure_speaker_ready();   // from bluetooth.c
    button_init_all();    // from Button_Control.c
    load_button_sounds_from_nvs();

    sound_reg_game_running = true;
    ESP_LOGI(TAG, "Starting Sound Recognition game!");

    static int current_playing_button = -1;

    while(sound_reg_game_running){
        for(int i = 0; i < NUM_BUTTONS; i++){
            if(button_is_pressed(i)){
                //stopping different button sounds
                if(current_playing_button != -1 && current_playing_button !=i){
                    ESP_LOGI(TAG, "Stopping sound for button %d", current_playing_button);
                    speaker_stop();
                }

                //starting new sound if none is playing
                if(current_playing_button !=i){
                    const char* sound_file = get_button_sound(i);
                    ESP_LOGI(TAG, "Button %d pressed! Playing sound: %s", i, sound_file);
                    play_button_sound(i);
                    sound_reg_points ++;
                    current_playing_button = i;
                    //evt_notify_sound_reg_result(sound_reg_points);
                }
                
                while(button_is_pressed(i)){ //wait for release
                    vTaskDelay(pdMS_TO_TICKS(50));
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(30));
    }
    speaker_stop();
    current_playing_button = -1;
}

static void LED_regular_game_task(void *pvParameters) {
    ensure_led_ready();   // from bluetooth.c
    button_init_all();    // from Button_Control.c
    load_button_states_from_nvs(); 
    led_clear();
    //irs_init();
    ensure_irs_ready();

    led_reg_game_running = true;
    ESP_LOGI(TAG, "Starting Regular LED game!");
    bool attempt_logged = false;

    while(led_reg_game_running){

        //hand near sensor behavior 
        if(irs_is_hand_near()){
            led_reg_interactions++;
            evt_notify_led_reg_results(led_reg_interactions);
            int i = 1;
            if(button_led_on[i]){
                button_led_on[i] = !button_led_on[i];
                ButtonColor color = get_button_color(i);
                for(int j = 0; j<NUM_BUTTONS;j++){
                    if(j==i){
                        set_button_color(i, 0, 0, 0, 0.0f);
                    }else{
                        set_button_color(j, 0, 0, 0, 0.0f);
                    }
                }
                set_button_color(i, color.r, color.g, color.b, color.brightness);
                ESP_LOGI(TAG, "Button %d → LED ON", i);
                ESP_LOGI(TAG, "Target %d -> r=%d g=%d b=%d br=%.2f",i, color.r, color.g, color.b, color.brightness);
            }else{
                button_led_on[i] = !button_led_on[i];
                set_button_color(i, 0, 0, 0, 0.0f);
                ESP_LOGI(TAG, "Button %d → LED OFF", i);
            }
            led_show();
            vTaskDelay(pdMS_TO_TICKS(500)); //debounce delay
        }

        //button press behavior
        for(int i = 0; i < NUM_BUTTONS; i++){
            if(button_is_pressed(i)){
                led_reg_interactions ++;
                evt_notify_led_reg_results(led_reg_interactions);
                if(!button_led_on[i]){
                    button_led_on[i] = !button_led_on[i];
                    ButtonColor color = get_button_color(i);
                    for(int j = 0; j<NUM_BUTTONS;j++){
                        if(j==i){
                            set_button_color(i, 0, 0, 0, 0.0f);
                        }else{
                            set_button_color(j, 0, 0, 0, 0.0f);
                        }
                    }
                    set_button_color(i, color.r, color.g, color.b, color.brightness);
                    ESP_LOGI(TAG, "Button %d → LED ON", i);
                    ESP_LOGI(TAG, "Target %d -> r=%d g=%d b=%d br=%.2f",i, color.r, color.g, color.b, color.brightness);
                }else{
                    button_led_on[i] = !button_led_on[i];
                    set_button_color(i, 0, 0, 0, 0.0f);
                    ESP_LOGI(TAG, "Button %d → LED OFF", i);
                }

                while(button_is_pressed(i)){ //wait for release
                    vTaskDelay(pdMS_TO_TICKS(50));
                }

                led_show();
            }
            
        }
        vTaskDelay(pdMS_TO_TICKS(30));
    }
    led_clear();
    led_show();

    //led_reg_game_running = false;
    game_task_handle = NULL;
    vTaskDelete(NULL);
}

static void whackamole_game_task(void *pvParameters) {
    ensure_led_ready();   // from bluetooth.c
    button_init_all();    // from Button_Control.c
    load_button_states_from_nvs(); 
    led_clear();
    //irs_init();
    ensure_irs_ready();

    whack_game_running = true;
    ESP_LOGI(TAG, "Starting Whack-A-Mole game!");

    while (whack_game_running) {
        // Choose a random button 0–3
        int target = esp_random() % NUM_BUTTONS;
        bool attempt_logged = false;

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

        int elapsed = 0;
        const int TIMEOUT_MS = 3000;

        // Wait for correct button press
        while (whack_game_running && elapsed < TIMEOUT_MS) {
            bool near = irs_is_hand_near();  
            bool pressed = button_is_pressed(target);
            //ESP_LOGI(TAG, "inside while loop");

            if (near && !attempt_logged && !pressed) {
                vTaskDelay(pdMS_TO_TICKS(50));
                led_mode_attempts ++;
                evt_notify_led_whack_result(led_mode_points, led_mode_attempts);
                attempt_logged = true;
                ESP_LOGI(TAG, "Attempt detected for button %d", target);
                
            }
            
            if(pressed){
                vTaskDelay(pdMS_TO_TICKS(50));
                ESP_LOGI(TAG, "in pressed loop");
                led_mode_points ++;
                evt_notify_led_whack_result(led_mode_points, led_mode_attempts);
                ESP_LOGI(TAG, "Button %d pressed! (+1 point)", target);
                // Turn off LEDs for that button
                set_button_color(target, 0, 0, 0, 0.0f);
                led_show();
                vTaskDelay(pdMS_TO_TICKS(250)); // small pause
                //break; // move to next round
                goto NEXT_ROUND;
            }
            vTaskDelay(pdMS_TO_TICKS(50)); // polling interval
        
            vTaskDelay(pdMS_TO_TICKS(50));
            elapsed += 50;
        }
        ESP_LOGI(TAG, "Timeout – no press for target %d", target);
        NEXT_ROUND:
        // Turn off LEDs for that button
            set_button_color(target, 0, 0, 0, 0.0f);
            led_show();
            vTaskDelay(pdMS_TO_TICKS(500)); // small pause before next round
    }

    // Cleanup when game stops
    led_clear();
    led_show();
    ESP_LOGI(TAG, "Whack-A-Mole stopped.");
    // Set handle to NULL before delete
    //whack_game_running = false;
    game_task_handle = NULL;
    vTaskDelete(NULL);
}

// ================= PUBLIC FUNCTIONS =================
void start_led_reg_game(void){
    if(game_task_handle == NULL){
        led_reg_interactions = 0;
        evt_notify_led_reg_results(led_reg_interactions);
        for (int i = 0; i < NUM_BUTTONS; i++) {
            button_led_on[i] = false;
        }
        xTaskCreate(
            LED_regular_game_task,
            "LED_regular_game_task",
            4096,
            NULL,
            5,
            &game_task_handle
        );
    } else {
        ESP_LOGW(TAG, "in leg_reg start: Whack-A-Mole or another game running already running.");
    }
}

void stop_led_reg_game(void) {
    if (game_task_handle != NULL) {
        ESP_LOGI(TAG, "Stopping LED-reg-game..");
        led_reg_game_running = false;   // signal to end loop
    }
    game_task_handle = NULL;
}

void start_sound_game(void){
    if(game_task_handle == NULL){
        sound_reg_points = 0;
        xTaskCreate(
            sound_game_task,
            "sound_game_task",
            4096,
            NULL,
            5,
            &game_task_handle
        );
    }else{
        ESP_LOGW(TAG, "in sound reg start: Whack-A-Mole or another game running already running.");
    }
}

void stop_sound_game(void){
    if (game_task_handle != NULL) {
        ESP_LOGI(TAG, "Stopping Sound Recognition game..");
        sound_reg_game_running = false;   // signal to end loop
    }
    game_task_handle = NULL;
}

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
        ESP_LOGW(TAG, "in whack start: Whack-A-Mole or another game running already running.");
    }
}

void stop_whackamole_game(void) {
    if (game_task_handle != NULL) {
        ESP_LOGI(TAG, "Stopping Whack-A-Mole...");
        whack_game_running = false;   // signal to end loop
    }
    game_task_handle = NULL;
}
