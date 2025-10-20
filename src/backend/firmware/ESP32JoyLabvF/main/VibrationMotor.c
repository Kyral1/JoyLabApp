#include "VibrationMotor.h"
#include "esp_log.h"

static const char *TAG = "VIBRATION";
static bool motor_state = false;

void vibration_init(void){
    gpio_config_t io_conf= {
        .pin_bit_mask = 1ULL << VIBRATION_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0, 
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    gpio_set_level(VIBRATION_GPIO, 0);
    motor_state = false;
    ESP_LOGI(TAG, "Vibration motor initialized on GPIO %d", VIBRATION_GPIO);
}

void vibration_set_state(bool on){
    motor_state = on; 
    gpio_set_level(VIBRATION_GPIO, on ? 1 :0);
    ESP_LOGI(TAG, "Vibration motor %s", on ? "ON" : "OFF");
}

bool vibration_get_state(void){
    return motor_state;
}