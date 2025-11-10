idf_component_register(
    SRCS "main.c" "src/Bluetooth.c" "src/Button_Control.c" "src/dotStar.c" "src/ForceSensor_Control.c" "src/IRS_Control.c" "src/LED_Control.c" "src/Speaker_Control.c" "src/VibrationMotor.c" "vl53l0x/VL53L0X_1.0.4/Api/core/src/vl53l0x_api.c"
        "vl53l0x/VL53L0X_1.0.4/Api/core/src/vl53l0x_api_calibration.c"
        "vl53l0x/VL53L0X_1.0.4/Api/core/src/vl53l0x_api_core.c"
        "vl53l0x/VL53L0X_1.0.4/Api/core/src/vl53l0x_api_ranging.c"
        "vl53l0x/VL53L0X_1.0.4/Api/core/src/vl53l0x_api_strings.c"
        "vl53l0x/VL53L0X_1.0.4/Api/platform/src/vl53l0x_i2c_platform.c"
        "vl53l0x/VL53L0X_1.0.4/Api/platform/src/vl53l0x_platform.c"
        "vl53l0x/VL53L0X_1.0.4/Api/platform/src/vl53l0x_platform_log.c"
        "src/Game_Control.c" 
    INCLUDE_DIRS "include" "vl53l0x/VL53L0X_1.0.4/Api/core/inc" "vl53l0x/VL53L0X_1.0.4/Api/platform/inc"
    REQUIRES driver nvs_flash esp_timer bt esp_system spiffs
)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-maybe-uninitialized)

