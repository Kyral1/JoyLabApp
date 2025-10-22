#include "driver/spi_master.h"
#include "esp_log.h"

#define DATA_PIN   23   // DotStar DI 
#define CLOCK_PIN  18   // DotStar CI 
#define NUM_LEDS   72

static const char *TAG = "DOTSTAR";
static spi_device_handle_t spi;

/* Initialize SPI bus for APA102/DotStar */
void dotstar_init(void) {
    spi_bus_config_t buscfg = {
        .mosi_io_num = DATA_PIN,
        .miso_io_num = -1,
        .sclk_io_num = CLOCK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 4000000,  // 4 MHz is safe
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 1,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi));
    ESP_LOGI(TAG, "DotStar SPI initialized");
}

/* Send start, LED, and end frames */
void dotstar_show(uint8_t *rgb, int count) {
    spi_transaction_t t = {0};

    // Start frame
    uint8_t start[4] = {0,0,0,0};
    t.length = 32;
    t.tx_buffer = start;
    spi_device_transmit(spi, &t);

    // LED frames (0xFF + BGR)
    for (int i = 0; i < count; i++) {
        uint8_t frame[4] = {0xFF, rgb[3*i+2], rgb[3*i+1], rgb[3*i+0]};
        t.length = 32;
        t.tx_buffer = frame;
        spi_device_transmit(spi, &t);
    }

    // End frame (at least n/2 bits of 1’s)
    uint8_t end[4] = {0xFF,0xFF,0xFF,0xFF};
    t.length = 32;
    t.tx_buffer = end;
    spi_device_transmit(spi, &t);

    ESP_LOGI(TAG, "DotStar data sent");
}
