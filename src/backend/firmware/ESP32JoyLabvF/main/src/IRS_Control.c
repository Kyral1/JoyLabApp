#include "IRS_Control.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "esp_log.h"

static const char *TAG = "IRS_CONTROL";
static VL53L0X_Dev_t sensor;
#define PROXIMITY_THRESHOLD_MM 100  // Adjust this for sensitivity (e.g., 50–150mm)

void irs_init(void) {
    ESP_LOGI(TAG, "Initializing VL53L0X...");
    sensor.I2cDevAddr = 0x29;
    VL53L0X_DataInit(&sensor);
    VL53L0X_StaticInit(&sensor);
    VL53L0X_PerformRefCalibration(&sensor, NULL, NULL);
    VL53L0X_PerformRefSpadManagement(&sensor, NULL, NULL);
    VL53L0X_SetDeviceMode(&sensor, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING);
    VL53L0X_StartMeasurement(&sensor);
}

uint16_t irs_read_distance_mm(void) {
    VL53L0X_RangingMeasurementData_t measure;
    VL53L0X_GetRangingMeasurementData(&sensor, &measure);
    ESP_LOGI(TAG, "Distance: %d mm", measure.RangeMilliMeter);
    VL53L0X_ClearInterruptMask(&sensor, 0);
    return measure.RangeMilliMeter;
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