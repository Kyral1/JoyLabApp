#include "VibrationMotor.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

static const char *TAG = "VIBRATION";
static bool motor_state = false;

// Motor 1: GPIO16
#define MOTOR1_PWM_PIN     16
#define MOTOR1_CHANNEL     LEDC_CHANNEL_0

// Motor 2: GPIO17
#define MOTOR2_PWM_PIN     17
#define MOTOR2_CHANNEL     LEDC_CHANNEL_1

#define MOTOR_TIMER        LEDC_TIMER_0

static bool motor1_state = false;
static bool motor2_state = false;
static uint8_t motor1_intensity = 0;
static uint8_t motor2_intensity = 0;

void vibration_init(void){
    ledc_timer_config_t timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_8_BIT,
        .timer_num        = MOTOR_TIMER,
        .freq_hz          = 5000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    // Motor 1 PWM
    ledc_channel_config_t ch1 = {
        .gpio_num   = MOTOR1_PWM_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = MOTOR1_CHANNEL,
        .timer_sel  = MOTOR_TIMER,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&ch1);

    // Motor 2 PWM
    ledc_channel_config_t ch2 = {
        .gpio_num   = MOTOR2_PWM_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = MOTOR2_CHANNEL,
        .timer_sel  = MOTOR_TIMER,
        .duty       = 0,
        .hpoint     = 0
    };
    ledc_channel_config(&ch2);
    ESP_LOGI(TAG, "Vibration motor initialized");
}

// ===================== MOTOR 1 =====================
void vibration_set_state_motor1(bool on) {
    motor1_state = on;
    uint32_t duty = on ? (motor1_intensity * 255) / 100 : 0;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR1_CHANNEL, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR1_CHANNEL);
    ESP_LOGI(TAG, "Motor1 %s", on ? "ON" : "OFF");
}

void vibration_set_intensity_motor1(uint8_t intensity) {
    if (intensity > 100) intensity = 100;
    motor1_intensity = intensity;
    if (motor1_state) {
        uint32_t duty = (intensity * 255) / 100;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR1_CHANNEL, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR1_CHANNEL);
    }
    ESP_LOGI(TAG, "Motor1 intensity set to %d%%", intensity);
}

// ===================== MOTOR 2 =====================
void vibration_set_state_motor2(bool on) {
    motor2_state = on;
    uint32_t duty = on ? (motor2_intensity * 255) / 100 : 0;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR2_CHANNEL, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR2_CHANNEL);
    ESP_LOGI(TAG, "Motor2 %s", on ? "ON" : "OFF");
}

void vibration_set_intensity_motor2(uint8_t intensity) {
    if (intensity > 100) intensity = 100;
    motor2_intensity = intensity;
    if (motor2_state) {
        uint32_t duty = (intensity * 255) / 100;
        ledc_set_duty(LEDC_LOW_SPEED_MODE, MOTOR2_CHANNEL, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, MOTOR2_CHANNEL);
    }
    ESP_LOGI(TAG, "Motor2 intensity set to %d%%", intensity);
}