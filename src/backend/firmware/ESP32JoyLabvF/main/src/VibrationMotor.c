#include "VibrationMotor.h"
#include "esp_log.h"

static const char *TAG = "VIBRATION";
static bool motor_state = false;

typedef struct {
    gpio_num_t pin;
    bool state;
} VibrationMotor;

void vibration_init(gpio_num_t pin){
    gpio_config_t io_conf= {
        .pin_bit_mask = 1ULL << pin,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0, 
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    gpio_set_level(pin, 0);
    motor_state = false;
    ESP_LOGI(TAG, "Vibration motor initialized on GPIO %d", pin);
}

void vibration_set_state(bool on, gpio_num_t pin){
    gpio_set_level(pin, on ? 1 : 0);
    for (int i = 0; i < motor_count; i++) {
        if (motors[i].pin == pin) {
            motors[i].state = on;
            break;
        }
    }
    ESP_LOGI(TAG, "Motor on GPIO %d %s", pin, on ? "ON" : "OFF");
}

bool vibration_get_state(void){
    for (int i = 0; i < motor_count; i++) {
        if (motors[i].pin == pin) {
            return motors[i].state;
        }
    }
    return false; // default if not found
}