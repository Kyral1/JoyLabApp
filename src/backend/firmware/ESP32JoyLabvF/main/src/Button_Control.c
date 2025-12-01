//pulls in ESP-IDF’s GPIO driver library
#include "driver/gpio.h"
#include "Button_Control.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//log tag
static const char *TAG = "BUTTON";
#define DEBOUNCE_DELAY_MS 50

//Button Pins Mapping (change when ESP is configured)
const int button_pins[NUM_BUTTONS] = {4, 0, 26, 27};

//resets each pin before config
void button_init_all(void) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        gpio_reset_pin(button_pins[i]);
        gpio_set_direction(button_pins[i], GPIO_MODE_INPUT);
        gpio_pullup_dis(button_pins[i]);   // use internal pull-up
        gpio_pulldown_en(button_pins[i]);
        ESP_LOGI(TAG, "Button %d initialized on GPIO %d", i, button_pins[i]);
    }
}

//checks if button is pressed
bool button_is_pressed(int index) {
    if (index < 0 || index >= NUM_BUTTONS) return false;

    int first_read = gpio_get_level(button_pins[index]);
    vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_DELAY_MS));  // simple debounce
    int second_read = gpio_get_level(button_pins[index]);

    return (first_read == 1 && second_read == 1);
}