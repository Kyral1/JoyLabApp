#include "driver/i2c.h"
#include "esp_log.h"
#include "ForceSensor_Control.h"

#define FORCE_SENSOR_ADDR 0x28 //I2C address from FMA - I think this cannot be changed
#define I2C_PORT I2C_NUM_0
#define TAG "FORCE_SENSOR"

void force_sensor_init(void) {
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

