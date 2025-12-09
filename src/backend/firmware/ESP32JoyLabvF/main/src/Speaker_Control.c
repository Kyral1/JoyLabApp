#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#include "Speaker_Control.h"
#include "driver/dac.h"
#include "driver/i2s.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include <stdio.h>
#include "math.h"
#include "minimp3.h"
#include "minimp3_ex.h"

static const char *TAG = "SPEAKER";
#define SPEAKER_DAC_CHANNEL DAC_CHANNEL_1 //25
#define I2S_NUM I2S_NUM_0
static uint8_t speaker_volume = 0;
mp3dec_t mp3d;

/*void speaker_init(void) {
    dac_output_enable(SPEAKER_DAC_CHANNEL);
    dac_output_voltage(SPEAKER_DAC_CHANNEL, 0);
}*/

void speaker_init(void)
{
    // Configure I2S -> DAC
    i2s_config_t cfg = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
        .sample_rate = 22050,                    // must match WAV sample rate
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };

    ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL));
    i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN);  // DAC1 = GPIO25

    speaker_volume = 50;  // default volume

    ESP_LOGI(TAG, "Speaker initialized (embedded WAV mode)");
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

void speaker_play_mp3_file(const char *path){
    mp3dec_file_info_t info;
    memset(&info, 0, sizeof(info));

    int res = mp3dec_load(&mp3d, path, &info, NULL, NULL);
    if(res){
        ESP_LOGE(TAG, "Failed to load MP3 file: %s", path);
        return;
    }

    i2s_set_clk(I2S_NUM_0, info.hz, I2S_BITS_PER_SAMPLE_16BIT, 1);

    size_t written;
    for (int i = 0; i<info.samples; i++){
        int16_t s = info.buffer[i];
        uint8_t out = (uint8_t)((((s + 32768) >> 8) * speaker_volume) / 255);
        i2s_write(I2S_NUM_0, &out, 1, &written, portMAX_DELAY);
    }

    free(info.buffer);
    i2s_zero_dma_buffer(I2S_NUM_0);
    ESP_LOGI(TAG, "MP3 playback finished");
}

/*
void speaker_play_wav_mem(const uint8_t *data_start, const uint8_t *data_end)
{
    size_t len = data_end - data_start;
    const uint8_t *ptr = data_start + 44;   // skip WAV header
    const uint8_t *end = data_end;

    int16_t sample16;
    uint8_t out;
    size_t written;

    while (ptr + 2 <= end)
    {
        sample16 = ptr[0] | (ptr[1] << 8);  // little-endian 16-bit
        ptr += 2;

        int32_t s = sample16 + 32768;      // signed → unsigned
        s >>= 8;                           // 16bit → 8bit
        s = (s * speaker_volume) / 255;    // apply volume

        out = (uint8_t)s;
        i2s_write(I2S_NUM_0, &out, 1, &written, portMAX_DELAY);
    }

    i2s_zero_dma_buffer(I2S_NUM_0);
}*/

void speaker_beep_blocking(uint16_t freq_hz, uint32_t duration_ms)
{
    const int sample_rate = 22050;
    const int samples = (sample_rate * duration_ms) / 1000;

    float step = (2.0f * 3.14159265f * freq_hz) / sample_rate;
    float phase = 0;

    size_t written;
    uint8_t out;

    for (int i = 0; i < samples; i++)
    {
        float sample = (sinf(phase) * 0.5f + 0.5f) * speaker_volume;
        out = (uint8_t)(sample);

        i2s_write(I2S_NUM_0, &out, 1, &written, portMAX_DELAY);

        phase += step;
        if (phase > 2 * 3.14159265f) phase -= 2 * 3.14159265f;
    }

    i2s_zero_dma_buffer(I2S_NUM_0);
}

