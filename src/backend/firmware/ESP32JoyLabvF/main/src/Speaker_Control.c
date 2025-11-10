#include "Speaker_Control.h"
#include "driver/dac.h"
#include "driver/i2s.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include <stdio.h>


static const char *TAG = "SPEAKER";
#define SPEAKER_DAC_CHANNEL DAC_CHANNEL_1 //25
#define I2S_NUM I2S_NUM_0
static uint8_t speaker_volume = 0;

/*void speaker_init(void) {
    dac_output_enable(SPEAKER_DAC_CHANNEL);
    dac_output_voltage(SPEAKER_DAC_CHANNEL, 0);
}*/

void speaker_init(void)
{
    // Mount SPIFFS (for /spiffs/*.wav)
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));
    ESP_LOGI(TAG, "SPIFFS mounted at /spiffs");

    // Enable DAC (GPIO25) and configure I2S internal DAC
    i2s_config_t cfg = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
        .sample_rate = 22050,                    // match your .wav file
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };
    ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM, &cfg, 0, NULL));
    i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);  // DAC1 → GPIO25

    ESP_LOGI(TAG, "Speaker initialized: I2S→DAC(GPIO25)→AMP→Speaker");
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

void speaker_play_wav(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "❌ Failed to open %s", path);
        return;
    }

    // Skip the 44-byte WAV header (for PCM .wav)
    fseek(f, 44, SEEK_SET);

    uint8_t buf[1024];
    size_t bytes_read, bytes_written;
    ESP_LOGI(TAG, "▶ Playing WAV: %s", path);

    while ((bytes_read = fread(buf, 1, sizeof(buf), f)) > 0) {
        // Apply simple volume scaling
        for (size_t i = 0; i < bytes_read; i++) {
            int val = (buf[i] * speaker_volume) / 255;
            buf[i] = (uint8_t)val;
        }
        i2s_write(I2S_NUM, buf, bytes_read, &bytes_written, portMAX_DELAY);
    }

    fclose(f);
    i2s_zero_dma_buffer(I2S_NUM);
    ESP_LOGI(TAG, "✅ Playback finished: %s", path);
}
