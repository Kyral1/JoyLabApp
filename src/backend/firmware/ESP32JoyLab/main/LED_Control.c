//pulls in ESP-IDF’s GPIO driver library
#include "LED_Control.h"
#include "esp_log.h"
#include "led_strip.h"

//Log Tag - any messages from this file will have the tag below
static const char *TAG = "LED_CONTROL";

//object representing LED strip
static led_strip_handle_t led_strip;

//prepares all LEDs for use (reset + set as outputs)
void led_init(void){
    //dont have to include logs -- good for debugging
    ESP_LOGI(TAG, "Initializing DotStar LED strip");

    led_strip_config_t strip_config = { //describes data strip
        .strip_gpio_num = 5 //CHANGE later
        .max_leds = 4 //# in strip
    };

    led_strip_spi_config_t spi_config = { //using spi
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };

    //creating LED strip object and assigning it to LED_strip
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
    led_strip_clear(led_strip); //turns all LEDs OFF to start
}

//LED at position INDEX Color (turn ON)
void led_set_color_brightness(int index, uint8_t r, uint8_t g, uint8_t b, float brightness){
    //scale each RGB to correct brightness
    uint8_t r_scaled = (uint8_t)(r * brightness);
    uint8_t g_scaled = (uint8_t)(g * brightness);
    uint8_t b_scaled = (uint8_t)(b * brightness);

    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, index, r_scaled, g_scaled, b_scaled));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip));
}

//Single LED OFF
void led_clear_one(int index){
    ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, index, 0, 0, 0));
    ESP_ERROR_CHECK(led_strip_refresh(led_strip)); //send LED values over the wire
}
//ALL LEDs OFF
void led_clear(void) {
    ESP_ERROR_CHECK(led_strip_clear(led_strip));
}