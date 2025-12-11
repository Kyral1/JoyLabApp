#ifndef BLUETOOTH_H
#define BLUETOOTH_H
#pragma once

#include "esp_err.h"
#include "esp_gatts_api.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_bt_main.h"
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

// ===================== UUID DEFINITIONS ======================
// Main JoyLab BLE service and characteristics (match bluetooth.c)

#define SERVICE_UUID     0x1234  // Primary Service
#define CTRL_UUID        0xABCD  // Control Characteristic (Write NR)
#define SETT_UUID        0xABCE  // Settings Characteristic (Read/Write)
#define EVTS_UUID        0xABCF  // Events Characteristic (Notify)

// ===================== PUBLIC FUNCTIONS ======================

// Initializes the BLE stack, registers GATT callbacks, and starts advertising.
// Should be called once during system startup.
esp_err_t bluetooth_init(void);

// Deinitializes BLE and stops advertising (optional for shutdown/restart).
void bluetooth_deinit(void);

void ensure_led_ready(void);
void ensure_irs_ready(void);
void ensure_speaker_ready(void);
void ble_notify_distance(uint16_t distance_mm);
void evt_notify_led_whack_result(uint8_t points, uint8_t attempts);
void evt_notify_led_reg_results(uint8_t interactions);
void ensure_force_ready(void);

#ifdef __cplusplus
}
#endif
#endif // BLUETOOTH_H
