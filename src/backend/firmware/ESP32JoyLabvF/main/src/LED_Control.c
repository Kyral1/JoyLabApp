#include "LED_Control.h"
#include "dotstar.h"
#include "esp_log.h"
#include <string.h>
#include "nvs_flash.h"
#include "nvs.h"

#define NUM_LEDS 72
#define NUM_BUTTONS 4
static const char *TAG = "LED_CONTROL";

static uint8_t led_buffer[NUM_LEDS * 3];
static float global_brightness = 1.0f;

//default states: white full brightness
//the live state of the button
static ButtonColor button_states[NUM_BUTTONS] = {
    {255, 255, 255, 1.0f},  
    {255, 255, 255, 1.0f},
    {255, 255, 255, 1.0f},
    {255, 255, 255, 1.0f},
};

//the color the button is set to 
static ButtonColor button_colors[NUM_BUTTONS]={
    {255, 255, 255, 1.0f},  
    {255, 255, 255, 1.0f},
    {255, 255, 255, 1.0f},
    {255, 255, 255, 1.0f},
};

static const int buttonLEDs[4][9] = {
    {0, 1, 2, 3, 4, 5, 6, 7, -1},       // Button 0
    {8, 9, 10, 11, 12, 13, 14, 15, -1}, // Button 1
    {16, 17, 18, 19, 20, 21, 22, 23, -1}, // Button 2
    {24, 25, 26, 27, 28, 29, 30, 31, -1}  // Button 3
};

void save_button_state(int button, uint8_t r, uint8_t g, uint8_t b, float brightness) {
    if (button < 0 || button > 3) return;
    button_states[button] = (ButtonColor){r, g, b, brightness};
}

void save_button_color(int button, uint8_t r, uint8_t g, uint8_t b, float brightness){
    if (button < 0 || button > 3) return;
    button_colors[button] = (ButtonColor){r, g, b, brightness};
}

ButtonColor get_button_color(int button) {
    ButtonColor default_state = {255, 255, 255, 1.0f};
    if (button < 0 || button > 3) return default_state;
    return button_colors[button];
}

ButtonColor get_button_state(int button) {
    ButtonColor default_state = {255, 255, 255, 1.0f};
    if (button < 0 || button > 3) return default_state;
    return button_states[button];
}

/* Initialize DotStar strip */
void led_init(void) {
    ESP_LOGI(TAG, "Initializing LED control (DotStar backend)...");
    memset(led_buffer, 0, sizeof(led_buffer));
    dotstar_init();
    dotstar_show(led_buffer, NUM_LEDS);
}

/* Set color + brightness for one LED */
void led_set_color_brightness(int index, uint8_t r, uint8_t g, uint8_t b, float brightness) {
    if (index < 0 || index >= NUM_LEDS) return;

    float scale = brightness * global_brightness;
    if (scale > 1.0f) scale = 1.0f;
    if (scale < 0.0f) scale = 0.0f;

    led_buffer[3*index + 0] = (uint8_t)(r * scale);
    led_buffer[3*index + 1] = (uint8_t)(g * scale);
    led_buffer[3*index + 2] = (uint8_t)(b * scale);
}

/* Clear one LED */
void led_clear_one(int index) {
    if (index < 0 || index >= NUM_LEDS) return;
    led_buffer[3*index + 0] = 0;
    led_buffer[3*index + 1] = 0;
    led_buffer[3*index + 2] = 0;
}

/* Clear all LEDs */
void led_clear(void) {
    memset(led_buffer, 0, sizeof(led_buffer));
}

/* Set global brightness (0.0–1.0) */
void led_set_global_brightness(float brightness) {
    if (brightness < 0.0f) brightness = 0.0f;
    if (brightness > 1.0f) brightness = 1.0f;
    global_brightness = brightness;
}

/* Push the current buffer to LEDs */
void led_show(void) {
    dotstar_show(led_buffer, NUM_LEDS);
}

void set_button_color(int button_num, uint8_t r, uint8_t g, uint8_t b, float brightness) {
    if (button_num < 0 || button_num >=NUM_BUTTONS) return;  // safety check

    if(button_num == 111){
        for (int i = 0; i < NUM_BUTTONS; i++) {
            save_button_state(i, r, g, b, brightness);
            const int *led_list = buttonLEDs[i];
            for (int j = 0; led_list[j] != -1; j++) {
                led_set_color_brightness(led_list[j], r, g, b, brightness);
            }
        }
        return;
    }

    save_button_state(button_num, r, g, b, brightness);
    for (int i = 0; i < NUM_BUTTONS; i++) {
        ButtonColor state = get_button_state(i);
        const int *led_list = buttonLEDs[i];
        for (int j = 0; led_list[j] != -1; j++) {
            led_set_color_brightness(led_list[j], state.r, state.g, state.b, state.brightness);
        }
    }
}

void save_button_state_persistent(int button, uint8_t r, uint8_t g, uint8_t b, float brightness) {
    nvs_handle_t nvs;
    if (nvs_open("led_states", NVS_READWRITE, &nvs) == ESP_OK) {
        uint32_t packed = (r << 24) | (g << 16) | (b << 8) | (uint8_t)(brightness * 100);
        char key[8];
        sprintf(key, "btn%d", button);
        nvs_set_u32(nvs, key, packed);
        nvs_commit(nvs);
        nvs_close(nvs);
    }
}

void load_button_states_from_nvs(void) {
    nvs_handle_t nvs;
    if (nvs_open("led_states", NVS_READONLY, &nvs) == ESP_OK) {
        for (int i = 0; i < NUM_BUTTONS; i++) {
            char key[8];
            sprintf(key, "btn%d", i);
            uint32_t packed;
            if (nvs_get_u32(nvs, key, &packed) == ESP_OK) {
                uint8_t r  = (packed >> 24) & 0xFF;
                uint8_t g  = (packed >> 16) & 0xFF;
                uint8_t b  = (packed >> 8)  & 0xFF;
                float br   = (packed & 0xFF) / 100.0f;
                save_button_state(i, r, g, b, br);
            }
        }
        nvs_close(nvs);
    }
}


