#include "VibrationMotor.h"
#include "esp_log.h"
#include "driver/mcpwm.h"
#include "driver/gpio.h"

static const char *TAG = "VIBRATION";

// Motor pins (unchanged)
#define MOTOR1_PWM_PIN     16
#define MOTOR2_PWM_PIN     17

// MCPWM unit/timer
#define VIB_MCPWM_UNIT     MCPWM_UNIT_0
#define VIB_MCPWM_TIMER    MCPWM_TIMER_0

// Internal state
static bool motor1_state = false;
static bool motor2_state = false;
static uint8_t motor1_intensity = 0;   // 0-100%
static uint8_t motor2_intensity = 0;   // 0-100%

// ============================================================
// INITIALIZATION
// ============================================================
void vibration_init(void)
{
    // Configure GPIO pins
    mcpwm_gpio_init(VIB_MCPWM_UNIT, MCPWM0A, MOTOR1_PWM_PIN); // Motor1
    mcpwm_gpio_init(VIB_MCPWM_UNIT, MCPWM0B, MOTOR2_PWM_PIN); // Motor2

    // Configure PWM timer
    mcpwm_config_t pwm_config = {
        .frequency = 5000,            // 5 kHz
        .cmpr_a = 0.0,                // Duty for Motor1 (0%)
        .cmpr_b = 0.0,                // Duty for Motor2 (0%)
        .counter_mode = MCPWM_UP_COUNTER,
        .duty_mode = MCPWM_DUTY_MODE_0,
    };

    mcpwm_init(VIB_MCPWM_UNIT, VIB_MCPWM_TIMER, &pwm_config);

    ESP_LOGI(TAG, "Vibration motor initialized (MCPWM)");
}

// ============================================================
// MOTOR 1 CONTROL
// ============================================================
void vibration_set_state_motor1(bool on)
{
    motor1_state = on;

    float duty = on ? motor1_intensity : 0;

    mcpwm_set_duty(VIB_MCPWM_UNIT, VIB_MCPWM_TIMER, MCPWM_OPR_A, duty);
    mcpwm_set_duty_type(VIB_MCPWM_UNIT, VIB_MCPWM_TIMER, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);

    ESP_LOGI(TAG, "Motor1 %s", on ? "ON" : "OFF");
}

void vibration_set_intensity_motor1(uint8_t intensity)
{
    if (intensity > 100) intensity = 100;

    motor1_intensity = intensity;

    if (motor1_state) {
        mcpwm_set_duty(VIB_MCPWM_UNIT, VIB_MCPWM_TIMER, MCPWM_OPR_A, intensity);
        mcpwm_set_duty_type(VIB_MCPWM_UNIT, VIB_MCPWM_TIMER, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    }

    ESP_LOGI(TAG, "Motor1 intensity set to %d%%", intensity);
}

// ============================================================
// MOTOR 2 CONTROL
// ============================================================
void vibration_set_state_motor2(bool on)
{
    motor2_state = on;

    float duty = on ? motor2_intensity : 0;

    mcpwm_set_duty(VIB_MCPWM_UNIT, VIB_MCPWM_TIMER, MCPWM_OPR_B, duty);
    mcpwm_set_duty_type(VIB_MCPWM_UNIT, VIB_MCPWM_TIMER, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);

    ESP_LOGI(TAG, "Motor2 %s", on ? "ON" : "OFF");
}

void vibration_set_intensity_motor2(uint8_t intensity)
{
    if (intensity > 100) intensity = 100;

    motor2_intensity = intensity;

    if (motor2_state) {
        mcpwm_set_duty(VIB_MCPWM_UNIT, VIB_MCPWM_TIMER, MCPWM_OPR_B, intensity);
        mcpwm_set_duty_type(VIB_MCPWM_UNIT, VIB_MCPWM_TIMER, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
    }

    ESP_LOGI(TAG, "Motor2 intensity set to %d%%", intensity);
}
