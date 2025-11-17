#include "IRS_Control.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "esp_log.h"
#include "vl53l0x.h"
#include "driver/gpio.h"

static const char *TAG = "IRS_CONTROL";
static VL53L0X_Dev_t sensor;

#define PROXIMITY_THRESHOLD_MM 100  // Adjust this for sensitivity (e.g., 50–150mm)

void irs_init(void) {
    gpio_set_direction(14, GPIO_MODE_OUTPUT);
    gpio_set_level(14, 1);
    vTaskDelay(pdMS_TO_TICKS(10));

    ESP_LOGI(TAG, "Initializing VL53L0X (ESP32 driver)...");
    sensor.I2cDevAddr = 0x29;
    VL53L0X_Error err = VL53L0X_Device_init(&sensor);
    if (err != VL53L0X_ERROR_NONE)
    {
        ESP_LOGE(TAG, "VL53L0X init failed (%d)", err);
        return;
    }

    ESP_LOGI(TAG, "VL53L0X ready!");

    /*
    ESP_LOGI(TAG, "Initializing VL53L0X...");
    ESP_LOGI(TAG, "VL53L0X SDA pin = %d", VL53L0X_get_sda_pin());
    ESP_LOGI(TAG, "VL53L0X SCL pin = %d", VL53L0X_get_scl_pin());
    memset(&sensor, 0, sizeof(sensor));

    sensor.I2cDevAddr = 0x52;
    sensor.comms_type = 1;        // 1 = I2C for ST driver
    sensor.comms_speed_khz = 400;

    VL53L0X_Error err = VL53L0X_comms_initialise(0, 400);
    if (err) {
        ESP_LOGE(TAG, "VL53L0X I2C init failed: %d", err);
        return;
    }

    vTaskDelay(pdMS_TO_TICKS(10));            // REQUIRED for VL53L0X
    VL53L0X_WaitDeviceBooted(&sensor); 

    // 4. Required calibration variables
    uint8_t vhv_settings = 0;
    uint8_t phase_cal = 0;
    uint32_t refSpadCount = 0;
    uint8_t isApertureSpads = 0;

    err = VL53L0X_DataInit(&sensor);
    if (err) ESP_LOGE(TAG, "DataInit failed: %d", err);

    err = VL53L0X_StaticInit(&sensor);
    if (err) ESP_LOGE(TAG, "StaticInit failed: %d", err);

    err = VL53L0X_PerformRefCalibration(&sensor, &vhv_settings, &phase_cal);
    if (err) ESP_LOGE(TAG, "RefCalibration failed: %d", err);

    err = VL53L0X_PerformRefSpadManagement(&sensor, &refSpadCount, &isApertureSpads);
    if (err) ESP_LOGE(TAG, "RefSpadManagement failed: %d", err);

    // 6. Continuous ranging mode
    err = VL53L0X_SetDeviceMode(&sensor, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
    if (err) ESP_LOGE(TAG, "SetDeviceMode failed: %d", err);

    err = VL53L0X_StartMeasurement(&sensor);
    if (err) ESP_LOGE(TAG, "StartMeasurement failed: %d", err);

    ESP_LOGI(TAG, "VL53L0X ready in continuous mode");

    VL53L0X_DataInit(&sensor);
    VL53L0X_StaticInit(&sensor);
    VL53L0X_PerformRefCalibration(&sensor, NULL, NULL);
    VL53L0X_PerformRefSpadManagement(&sensor, NULL, NULL);
    VL53L0X_SetDeviceMode(&sensor, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
    VL53L0X_StartMeasurement(&sensor);
    */
}

uint16_t irs_read_distance_mm(void) {
    /*
    VL53L0X_RangingMeasurementData_t measure;
    VL53L0X_GetRangingMeasurementData(&sensor, &measure);
    ESP_LOGI(TAG, "Distance: %d mm", measure.RangeMilliMeter);
    VL53L0X_ClearInterruptMask(&sensor, 0);
    return measure.RangeMilliMeter;*/
    //irs_init();
    uint16_t dist = 0;
    if (VL53L0X_Device_getMeasurement(&sensor, &dist) == VL53L0X_ERROR_NONE)
    {
        ESP_LOGI(TAG, "Distance: %d mm", dist);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to read distance");
    }
    return dist;
}

bool irs_is_hand_near(void) {
    VL53L0X_RangingMeasurementData_t measure;
    VL53L0X_GetRangingMeasurementData(&sensor, &measure);

    if (measure.RangeStatus == 0 && measure.RangeMilliMeter < PROXIMITY_THRESHOLD_MM) {
        VL53L0X_ClearInterruptMask(&sensor, 0);
        return true;
    }
    VL53L0X_ClearInterruptMask(&sensor, 0);
    return false;
}