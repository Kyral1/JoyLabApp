#include "driver/i2c.h"
#include "esp_log.h"
#include "ForceSensor_Control.h"

#define FORCE_SENSOR_ADDR 0x28 //I2C address from FMA Datasheet 
#define I2C_PORT I2C_NUM_0
#define SDA_PIN 21
#define SCL_PIN 22
#define TAG "FORCE_SENSOR"

void force_sensor_init(void) {
    i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = SDA_PIN,         // select GPIO specific to your project
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = SCL_PIN,         // select GPIO specific to your project
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = 100000 //100kHz -- standard
    // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
};
    ESP_LOGI(TAG, "Force sensor initialized (I2C 0x%02X)", FORCE_SENSOR_ADDR);
}

float force_sensor_read(){
    uint8_t data[2];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, FORCE_SENSOR_ADDR << 1 | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 2, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);

    uint16_t raw = ((data[0] & 0x3F) << 8) | data[1];
    float pressure = ((float)raw / 16383.0f) * 25.0f;

    return pressure;
}

