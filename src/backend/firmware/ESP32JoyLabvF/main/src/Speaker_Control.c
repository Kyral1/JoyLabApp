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
static uint8_t volume_percent = 0;
static float volume_scale = 0.5f;
static volatile bool stop_audio_flag = false;

static TaskHandle_t speaker_task = NULL;

static uint8_t mp3_input_buf[2048];
static mp3d_sample_t mp3_pcm_buf[MINIMP3_MAX_SAMPLES_PER_FRAME];
static mp3dec_t mp3_decoder;
mp3dec_t mp3d;

typedef struct {
    uint32_t sample_rate;
    uint16_t bits_per_sample;
    uint16_t num_channels;
    uint32_t data_offset;
    uint32_t data_size;
} wav_info_t;

static int parse_wav_header(FILE *f, wav_info_t *info)
{
    uint8_t header[44];
    if (fread(header, 1, 44, f) != 44) {
        return -1;
    }

    info->sample_rate     = *(uint32_t *)&header[24];
    info->bits_per_sample = *(uint16_t *)&header[34];
    info->num_channels    = *(uint16_t *)&header[22];
    info->data_size       = *(uint32_t *)&header[40];
    info->data_offset     = 44;

    ESP_LOGI(TAG, "WAV info: %d Hz, %d bits, %d channels, %d bytes data",
             info->sample_rate, info->bits_per_sample,
             info->num_channels, info->data_size);

    return 0;
}

//working
void speaker_init(void)
{
    // Always attempt to uninstall first — safe even if not installed
    esp_err_t err = i2s_driver_uninstall(I2S_NUM_0);
    if (err == ESP_OK) {
        ESP_LOGW(TAG, "Previous I2S driver removed");
    } else {
        // ESP_ERR_INVALID_STATE means no driver existed — not a problem
        ESP_LOGI(TAG, "No previous I2S driver (OK)");
    }

    // Configure I2S -> DAC
    i2s_config_t cfg = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
        .sample_rate = 22050,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 2,
        .dma_buf_len = 512,
        .use_apll = true,
        .tx_desc_auto_clear = true,
    };

    ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL));
    ESP_ERROR_CHECK(i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN));

    speaker_volume = 50;

    ESP_LOGI(TAG, "Speaker initialized (safe reset mode)");
}

void speaker_set_volume(uint8_t level) {
    if (level > 100) level = 100;
    volume_scale = (float)level / 100.0f;

    ESP_LOGI(TAG, "Volume set to %d%%", level);
}

//working
uint8_t speaker_get_volume(void) {
    return (uint8_t)(volume_scale * 100);
}

void speaker_mute(void) {
    dac_output_voltage(SPEAKER_DAC_CHANNEL, 0);
    ESP_LOGI(TAG, "Speaker muted (DAC output 0V).");
}

void speaker_stop(void) {
    stop_audio_flag = true;
    i2s_zero_dma_buffer(I2S_NUM_0);
}

// Task function must match FreeRTOS signature
static void speaker_play_wav_task(void *path_ptr)
{
    const char *path = (const char *)path_ptr;

    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "Cannot open %s", path);
        speaker_task = NULL;
        vTaskDelete(NULL);
    }

    wav_info_t info;
    if (parse_wav_header(f, &info) != 0) {
        ESP_LOGE(TAG, "Invalid WAV header");
        fclose(f);
        speaker_task = NULL;
        vTaskDelete(NULL);
    }

    if (info.sample_rate < 16000) {
        info.sample_rate = 16000;
    }
    if (info.bits_per_sample != 8 && info.bits_per_sample != 16) {
        info.bits_per_sample = 16;
    }

    ESP_ERROR_CHECK(i2s_set_clk(I2S_NUM_0, info.sample_rate, info.bits_per_sample, 1));

    uint8_t buffer[512];
    size_t bytes_read, written;
    stop_audio_flag = false;

    while (!stop_audio_flag &&
           (bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0)
    {
        for (size_t i = 0; i < bytes_read; i++) {
            buffer[i] = (uint8_t)(buffer[i] * volume_scale);
        }

        //i2s_write(I2S_NUM_0, buffer, bytes_read, &written, 0);
        i2s_write(I2S_NUM_0, buffer, bytes_read, &written, portMAX_DELAY);
    }

    fclose(f);
    i2s_zero_dma_buffer(I2S_NUM_0);
    speaker_task = NULL;

    ESP_LOGI(TAG, "WAV finished");

    vTaskDelete(NULL);
}


void speaker_play_wav(const char *filename)
{
    static char fullpath[64];
    if (filename[0] == '/') {
        snprintf(fullpath, sizeof(fullpath), "%s", filename);
    }else{
        snprintf(fullpath, sizeof(fullpath), "/spiffs/%s", filename);
    }
    speaker_stop();

    xTaskCreate(
        speaker_play_wav_task,
        "speaker_task",
        4096,
        (void*)fullpath,
        5,
        &speaker_task
    );
}


void speaker_stop_task(void)
{
    stop_audio_flag = true;

    if (speaker_task != NULL) {
        vTaskDelay(pdMS_TO_TICKS(10));
        i2s_zero_dma_buffer(I2S_NUM_0);
        speaker_task = NULL;
    }
}



//working
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
