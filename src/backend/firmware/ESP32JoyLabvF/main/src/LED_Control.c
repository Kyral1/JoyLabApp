#include "LED_Control.h"
#include "dotstar.h"
#include "esp_log.h"
#include <string.h>

#define NUM_LEDS 72
#define NUM_BUTTONS 4
static const char *TAG = "LED_CONTROL";

static uint8_t led_buffer[NUM_LEDS * 3];
static float global_brightness = 1.0f;

//default states: white full brightness
static ButtonColor button_states[NUM_BUTTONS] = {
    {255, 255, 255, 1.0f},  
    {255, 255, 255, 1.0f},
    {255, 255, 255, 1.0f},
    {255, 255, 255, 1.0f},
};

void save_button_state(int button, uint8_t r, uint8_t g, uint8_t b, float brightness) {
    if (button < 0 || button > 3) return;
    button_states[button] = (ButtonColor){r, g, b, brightness};
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
    if (button_num < 0 || button_num > 3) return;  // safety check

    if(button_num == 111){
    for (int i = 0; i < 72; i++) {
        led_set_color_brightness(i, r, g, b, brightness);
    }
    return;
    }

    int leds_per_button = 10;
    int start = (button_num) * leds_per_button;

    for (int i = start; i < start + leds_per_button; i++) {
        led_set_color_brightness(i, r, g, b, brightness);
    }
}


