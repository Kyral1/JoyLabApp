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
static volatile bool stop_audio_flag = false;

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
        .dma_buf_count = 6,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };

    ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL));
    ESP_ERROR_CHECK(i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN));

    speaker_volume = 50;

    ESP_LOGI(TAG, "Speaker initialized (safe reset mode)");
}

//working
void speaker_set_volume(uint8_t level) {
    if (level > 100) level = 100;
    uint8_t dac_val = (level * 255) / 100;
    speaker_volume = dac_val;
    dac_output_voltage(SPEAKER_DAC_CHANNEL, dac_val);
    ESP_LOGI(TAG, "Speaker volume set to %d%%", speaker_volume);
}

//working
uint8_t speaker_get_volume(void) {
    return (speaker_volume * 100) / 255;
}

void speaker_mute(void) {
    dac_output_voltage(SPEAKER_DAC_CHANNEL, 0);
    ESP_LOGI(TAG, "Speaker muted (DAC output 0V).");
}

void speaker_stop(void) {
    stop_audio_flag = true;
    i2s_zero_dma_buffer(I2S_NUM_0);
}

void speaker_play_wav(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "Cannot open %s", path);
        return;
    }

    wav_info_t info;
    if (parse_wav_header(f, &info) != 0) {
        ESP_LOGE(TAG, "Invalid WAV header");
        fclose(f);
        return;
    }

    i2s_set_clk(I2S_NUM, info.sample_rate, info.bits_per_sample, 1);

    uint8_t buffer[512];
    size_t bytes_read, written;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        i2s_write(I2S_NUM, buffer, bytes_read, &written, portMAX_DELAY);
    }

    fclose(f);
    ESP_LOGI(TAG, "WAV finished");
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
/*
void speaker_play_mp3_file(const char *path)
{
    mp3dec_file_info_t info;
    memset(&info, 0, sizeof(info));

    int res = mp3dec_load(&mp3d, path, &info, NULL, NULL);

    if (res != 0) {
        ESP_LOGE(TAG, "Failed to load MP3: %s", path);
        return;
    }

    ESP_LOGI(TAG, "Playing %s (%d samples @ %d Hz)",
             path, info.samples, info.hz);

    // Set I2S clock to match MP3 sample rate
    i2s_set_clk(I2S_NUM, info.hz, I2S_BITS_PER_SAMPLE_16BIT, 1);

    size_t written;
    stop_audio_flag = false;

    for (int i = 0; i < info.samples; i++)
    {
        if (stop_audio_flag) break;

        int16_t s = info.buffer[i];

        // Convert signed 16-bit PCM → 8-bit DAC range (0–255)
        uint8_t out = (uint8_t)((((s + 32768) >> 8) * speaker_volume) / 100);

        // Write 8-bit sample directly
        i2s_write(I2S_NUM, &out, 1, &written, portMAX_DELAY);
    }

    free(info.buffer);
    i2s_zero_dma_buffer(I2S_NUM);

    ESP_LOGI(TAG, "MP3 playback finished");
}

//streaming task
static void speaker_mp3_stream_task(void *arg)
{
    const char *path = (const char *)arg;

    while (1)   // LOOP FOREVER SAFELY
    {
        FILE *f = fopen(path, "rb");
        if (!f) {
            ESP_LOGE("SPEAKER_STREAM", "Failed to open %s", path);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        // MUST re-init decoder every loop
        mp3dec_init(&mp3_decoder);

        mp3dec_frame_info_t info;
        size_t bytes_read = 0;

        while ((bytes_read = fread(mp3_input_buf, 1, sizeof(mp3_input_buf), f)) > 0)
        {
            int samples = mp3dec_decode_frame(
                &mp3_decoder,
                mp3_input_buf,
                bytes_read,
                mp3_pcm_buf,
                &info
            );

            if (samples <= 0) continue;

            size_t written;
            for (int i = 0; i < samples; i++)
            {
                int16_t s = mp3_pcm_buf[i];
                uint8_t out8 = (uint8_t)((((s + 32768) >> 8) * speaker_volume) / 255);
                uint16_t dac_sample = out8 << 8;

                i2s_write(I2S_NUM_0, &dac_sample, sizeof(dac_sample), &written, portMAX_DELAY);
            }

            // Allow scheduler to run!
            taskYIELD();
        }

        fclose(f);
        ESP_LOGI("SPEAKER_STREAM", "Loop finished — restarting file...");

        vTaskDelay(10 / portTICK_PERIOD_MS);  // tiny delay to avoid watchdog
    }

    vTaskDelete(NULL);  // never reached
}

void speaker_play_mp3_stream_safe(const char *path)
{
    xTaskCreatePinnedToCore(
        speaker_mp3_stream_task,
        "mp3_stream",
        20480,
        (void *)path,
        5,
        NULL,
        1
    );
}*/

/*#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3

#include "Speaker_Control.h"
#include "driver/i2s.h"
#include "driver/dac.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "minimp3.h"
#include "minimp3_ex.h"
#include <stdio.h>
#include <math.h>

static const char *TAG = "SPEAKER";

#define I2S_NUM I2S_NUM_0
#define SPEAKER_DAC_CHANNEL DAC_CHANNEL_1 // GPIO25

static uint8_t speaker_volume = 50;
static volatile bool stop_audio_flag = false;

static uint8_t mp3_input_buf[2048];
static mp3d_sample_t mp3_pcm_buf[MINIMP3_MAX_SAMPLES_PER_FRAME];
static mp3dec_t mp3_decoder;

mp3dec_t mp3d;

// =====================================================
// 1. SPEAKER INIT  — DO NOT MODIFY (you said it works)
// =====================================================
void speaker_init(void)
{
    i2s_config_t cfg = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
        .sample_rate = 22050,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 512,
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };

    ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM, &cfg, 0, NULL));
    i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN); // DAC1 (GPIO25)

    ESP_LOGI(TAG, "Speaker initialized");
}

// =====================================================
// 2. VOLUME / MUTE
// =====================================================
void speaker_set_volume(uint8_t level)
{
    if (level > 100) level = 100;
    speaker_volume = level;
    ESP_LOGI(TAG, "Speaker volume set to %d%%", speaker_volume);
}

uint8_t speaker_get_volume(void)
{
    return speaker_volume;
}

void speaker_mute(void)
{
    dac_output_voltage(SPEAKER_DAC_CHANNEL, 0);
}

void speaker_stop(void)
{
    stop_audio_flag = true;
    i2s_zero_dma_buffer(I2S_NUM);
}

// =====================================================
// 3. MP3 FILE LOAD (your original function)
// =====================================================
void speaker_play_mp3_file(const char *path)
{
    mp3dec_file_info_t info;
    memset(&info, 0, sizeof(info));

    int res = mp3dec_load(&mp3d, path, &info, NULL, NULL);
    if (res) {
        ESP_LOGE(TAG, "Failed to load MP3: %s", path);
        return;
    }

    i2s_set_clk(I2S_NUM, info.hz, I2S_BITS_PER_SAMPLE_16BIT, 1);

    size_t written;

    for (int i = 0; i < info.samples; i++)
    {
        int16_t s = info.buffer[i];
        uint8_t out8 = (uint8_t)((((s + 32768) >> 8) * speaker_volume) / 100);

        uint16_t dac_word = (out8 << 8);
        i2s_write(I2S_NUM, &dac_word, sizeof(dac_word), &written, portMAX_DELAY);
    }

    free(info.buffer);
    i2s_zero_dma_buffer(I2S_NUM);
}

// =====================================================
// 4. BLOCKING BEEP (unchanged)
// =====================================================
void speaker_beep_blocking(uint16_t freq_hz, uint32_t duration_ms)
{
    ESP_LOGI(TAG, "Beep: %d Hz for %d ms", freq_hz, duration_ms);
    
    int sample_rate = 22050;
    int samples = (sample_rate * duration_ms) / 1000;
    float step = (2.0f * 3.14159265f * freq_hz) / sample_rate;
    float phase = 0;

    size_t written;

    for (int i = 0; i < samples; i++)
    {
        float sample = (sinf(phase) * 0.5f + 0.5f);
        uint8_t out8 = (uint8_t)(sample * speaker_volume);

        uint16_t dac_word = out8 << 8;
        i2s_write(I2S_NUM, &dac_word, sizeof(dac_word), &written, portMAX_DELAY);

        phase += step;
        if (phase > 2 * M_PI) phase -= 2 * M_PI;

    }

    i2s_zero_dma_buffer(I2S_NUM);

}

// =====================================================
// 5. STREAMING TASK — FIXED (NO CRASHES)
// =====================================================
static void speaker_mp3_stream_task(void *arg)
{
    const char *path = (const char *)arg;

    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE("SPEAKER_STREAM", "Failed to open MP3: %s", path);
        vTaskDelete(NULL);
        return;
    }

    mp3dec_init(&mp3_decoder);

    mp3dec_frame_info_t info;
    size_t bytes_read = 0;

    while ((bytes_read = fread(mp3_input_buf, 1, sizeof(mp3_input_buf), f)) > 0)
    {
        int samples = mp3dec_decode_frame(
            &mp3_decoder,
            mp3_input_buf,
            bytes_read,
            mp3_pcm_buf,
            &info
        );

        if (samples <= 0) continue;

        size_t written;
        for (int i = 0; i < samples; i++)
        {
            int16_t s = mp3_pcm_buf[i];
            uint8_t out8 = (uint8_t)((((s + 32768) >> 8) * speaker_volume) / 255);
            uint16_t dac_sample = out8 << 8;

            i2s_write(I2S_NUM_0, &dac_sample, sizeof(dac_sample), &written, portMAX_DELAY);
        }
    }

    fclose(f);
    ESP_LOGI("SPEAKER_STREAM", "Finished streaming MP3.");
    vTaskDelete(NULL);
}


void speaker_play_mp3_stream_safe(const char *path)
{
    xTaskCreatePinnedToCore(
        speaker_mp3_stream_task,
        "mp3_stream",
        20480,
        (void *)path,
        5,
        NULL,
        1
    );
}*/


/*#define MINIMP3_IMPLEMENTATION
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
static volatile bool stop_audio_flag = false;
mp3dec_t mp3d;

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

void speaker_stop(void) {
    stop_audio_flag = true;
    i2s_zero_dma_buffer(I2S_NUM_0);
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
        if(stop_audio_flag){
            ESP_LOGI(TAG, "MP3 playback stopped.");
            break;
        }
        int16_t s = info.buffer[i];
        uint8_t out = (uint8_t)((((s + 32768) >> 8) * speaker_volume) / 255);
        i2s_write(I2S_NUM_0, &out, 1, &written, portMAX_DELAY);
    }

    free(info.buffer);
    i2s_zero_dma_buffer(I2S_NUM_0);
    stop_audio_flag = false;
    ESP_LOGI(TAG, "MP3 playback finished");
}
void speaker_play_mp3_stream(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "Cannot open file %s", path);
        return;
    }

    mp3dec_t mp3d;
    mp3dec_init(&mp3d);

    uint8_t input_buf[4096];
    mp3d_sample_t pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
    mp3dec_frame_info_t info;

    size_t bytes_read;
    while ((bytes_read = fread(input_buf, 1, sizeof(input_buf), f)) > 0) {

        int samples = mp3dec_decode_frame(
            &mp3d,
            input_buf,
            bytes_read,
            pcm,
            &info
        );

        if (samples > 0) {
            size_t written;
            i2s_write(I2S_NUM_0, pcm, samples * sizeof(int16_t), &written, portMAX_DELAY);
        }
    }

    fclose(f);
}


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

// ================================================================
// MP3 STREAMING TASK  — runs on its own FreeRTOS task
// ================================================================
static void speaker_mp3_stream_task(void *arg)
{
    const char *path = (const char *)arg;

    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE("SPEAKER_STREAM", "Cannot open %s", path);
        vTaskDelete(NULL);
        return;
    }

    mp3dec_t decoder;
    mp3dec_frame_info_t info;
    mp3dec_init(&decoder);

    uint8_t input_buf[4096];
    mp3d_sample_t pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];

    size_t bytes_read;

    while ((bytes_read = fread(input_buf, 1, sizeof(input_buf), f)) > 0)
    {
        int samples = mp3dec_decode_frame(
            &decoder,
            input_buf,
            bytes_read,
            pcm,
            &info
        );

        if (samples > 0)
        {
            // Convert 16-bit PCM → 8-bit DAC
uint16_t dac_sample;
size_t written;

for (int i = 0; i < samples; i++)
{
    // convert 16-bit PCM → 8-bit → expand to 16-bit for I2S DAC
    int16_t s = pcm[i];
    uint8_t out8 = (uint8_t)((((s + 32768) >> 8) * speaker_volume) / 255);

    dac_sample = out8 << 8;   // DAC reads upper 8 bits

    i2s_write(I2S_NUM_0, &dac_sample, sizeof(dac_sample), &written, portMAX_DELAY);
}
        }

        vTaskDelay(1);  // prevent watchdog
    }

    fclose(f);
    ESP_LOGI("SPEAKER_STREAM", "MP3 playback finished.");

    vTaskDelete(NULL);
}

void speaker_play_mp3_stream_safe(const char *path)
{
    xTaskCreatePinnedToCore(
        speaker_mp3_stream_task,
        "mp3_stream_task",
        6000,
        (void *)path,
        5,
        NULL,
        1   // play on core 1, leave BLE on core 0
    );
}*/
