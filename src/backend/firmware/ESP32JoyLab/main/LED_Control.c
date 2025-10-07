#include "LED_Control.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "led_strip.h"

// Log tag for debugging messages
static const char *TAG = "LED_CONTROL";

// Object representing the LED strip
static led_strip_handle_t led_strip;

// Initialize the LED strip
void led_init(void) {
    ESP_LOGI(TAG, "Initializing DotStar LED strip...");

    // --- SPI Bus Config ---
    spi_bus_config_t buscfg = {
        .mosi_io_num = 12,   // Data
        .sclk_io_num = 13,   // Clock
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    // Initialize SPI bus, but don't crash if it's already initialized
    esp_err_t ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "SPI bus already initialized — continuing");
    } else {
        ESP_ERROR_CHECK(ret);
    }

    // --- LED Strip Config ---
    led_strip_config_t strip_config = {
        .strip_gpio_num = -1,  // SPI uses MOSI/SCLK, no GPIO needed here
        .max_leds = 4,
    };

    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };

    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
    ESP_ERROR_CHECK(led_strip_clear(led_strip));

    ESP_LOGI(TAG, "DotStar LED strip initialized successfully!");
}

// Set specific LED color with brightness scaling
void led_set_color_brightness(int index, uint8_t r, uint8_t g, uint8_t b, float brightness) {
    uint8_t r_scaled = (uint8_t)(r * brightness);
    uint8_t g_scaled = (uint8_t)(g * brightness);
    uint8_t b_scaled = (uint8_t)(b * brightness);

    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, index, r_scaled, g_scaled, b_scaled));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

// Turn off a single LED
void led_clear_one(int index) {
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, index, 0, 0, 0));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

// Turn off all LEDs
void led_clear(void) {
    ESP_ERROR_CHECK(led_strip_clear(led_strip));
}

// Adjust brightness for all LEDs
void led_set_global_brightness(float brightness) {
    if (brightness < 0.0f) brightness = 0.0f;
    if (brightness > 1.0f) brightness = 1.0f;

    for (int i = 0; i < 4; i++) {
        led_set_color_brightness(i, 255, 255, 255, brightness);
    }

    ESP_LOGI(TAG, "Global LED brightness set to %.2f", brightness);
}
