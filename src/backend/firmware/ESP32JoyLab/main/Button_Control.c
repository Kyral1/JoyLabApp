//pulls in ESP-IDF’s GPIO driver library
#include "driver/gpio.h"
#include "Button_Control.h"
#include "esp_log.h"

//log tag
static const char *TAG = "BUTTON";

//Button Pins Mapping (change when ESP is configured)
const int button_pins[NUM_BUTTONS] = {4, 5, 18, 19, 21};

//resets each pin before config
void button_init_all(void) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
        gpio_reset_pin(button_pins[i]);
        gpio_set_direction(button_pins[i], GPIO_MODE_INPUT);
        gpio_pullup_en(button_pins[i]);   // use internal pull-up
        gpio_pulldown_dis(button_pins[i]);
        ESP_LOGI(TAG, "Button %d initialized on GPIO %d", i, button_pins[i]);
    }
}

//checks if button is pressed
int button_is_pressed(int id){
    int pin = (id == 1) ? BUTTON1_PIN : (id == 2) ? BUTTON2_PIN : BUTTON3_PIN; //ternary op.
    return gpio_get_level(pin) == 0;
}