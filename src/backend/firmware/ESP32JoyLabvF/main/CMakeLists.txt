idf_component_register(
    SRCS "main.c" "src/Bluetooth.c" "src/Button_Control.c" "src/dotStar.c" "src/ForceSensor_Control.c" "src/IRS_Control.c" "src/LED_Control.c" "src/Speaker_Control.c" "src/VibrationMotor.c" 
    "src/Game_Control.c" 
    "../external/VL53L0X_1.0.4/Api/core/src/vl53l0x_api.c"
    "../external/VL53L0X_1.0.4/Api/core/src/vl53l0x_api_calibration.c"
    "../external/VL53L0X_1.0.4/Api/core/src/vl53l0x_api_core.c"
    "../external/VL53L0X_1.0.4/Api/core/src/vl53l0x_api_ranging.c"
    "../external/VL53L0X_1.0.4/Api/core/src/vl53l0x_api_strings.c"
    "../components/vl53l0x_esp32/platform/esp32/src/vl53l0x_i2c_platform.c"
    "../components/vl53l0x_esp32/platform/esp32/src/vl53l0x_platform.c"
    "../components/vl53l0x_esp32/platform/esp32/src/vl53l0x_platform_log.c"
    "../components/vl53l0x_esp32/src/vl53l0x.c"
    INCLUDE_DIRS "include" "../external/VL53L0X_1.0.4/Api/core/inc" "../external/VL53L0X_1.0.4/Api/platform/inc" "../components/vl53l0x_esp32/include" 
    REQUIRES driver nvs_flash esp_timer bt esp_system spiffs 
)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-maybe-uninitialized)