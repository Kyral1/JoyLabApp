#include "Speaker_Control.h"
#include "driver/gpio.h"
#include "esp_log.h"


static const char *TAG = "SPEAKER";
static bool speaker_state = false;
static uint8_t speaker_volume = 50;

void speaker_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << SPEAKER_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    gpio_set_level(SPEAKER_GPIO, 0);
    speaker_state = false;
    ESP_LOGI(TAG, "Speaker initialized on GPIO %d", SPEAKER_GPIO);
}

void speaker_set_state(bool on) {
    speaker_state = on;
    gpio_set_level(SPEAKER_GPIO, on ? 1 : 0);
    ESP_LOGI(TAG, "Speaker %s", on ? "ON" : "OFF");
}

bool speaker_get_state(void) {
    return speaker_state;
}

void speaker_set_volume(uint8_t level) {
    if (level > 100) level = 100;
    speaker_volume = level;
    //implement PWM intensity later once hardware supports it
    ESP_LOGI(TAG, "Speaker volume set to %d%%", speaker_volume);
}