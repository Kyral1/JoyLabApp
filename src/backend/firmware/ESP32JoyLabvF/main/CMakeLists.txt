idf_component_register(
    SRCS "main.c" "LED_Control.c" "Button_Control.c" "dotstar.c" "Bluetooth.c" "VibrationMotor.c"
    INCLUDE_DIRS "."
    REQUIRES driver nvs_flash esp_timer bt esp_system 
)
