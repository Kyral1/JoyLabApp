#include "Speaker_Control.h"
#include "driver/gpio.h"
#include "driver/dac.h"
#include "esp_log.h"


static const char *TAG = "SPEAKER";
#define SPEAKER_DAC_CHANNEL DAC_CHANNEL_1 //25
static uint8_t speaker_volume = 0;

void speaker_init(void) {
    dac_output_enable(SPEAKER_DAC_CHANNEL);
    dac_output_voltage(SPEAKER_DAC_CHANNEL, 0);
}

void speaker_set_volume(uint8_t level) {
    if (level > 100) level = 100;
    uint8_t dac_val = (level * 255) / 100;
    speaker_volume = dac_val;

    dac_output_voltage(SPEAKER_DAC_CHANNEL, dac_val);
    ESP_LOGI(TAG, "Speaker volume set to %d%%", speaker_volume);
}

uint8_t speaker_get_volume(void) {
    return (speaker_volume * 100) / 255;
}

void speaker_mute(void) {
    dac_output_voltage(SPEAKER_DAC_CHANNEL, 0);
    ESP_LOGI(TAG, "Speaker muted (DAC output 0V).");
}