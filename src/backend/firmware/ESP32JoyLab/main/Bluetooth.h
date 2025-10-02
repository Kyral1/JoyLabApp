#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "esp_gatts_api.h"
#include "esp_gap_ble_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

//srvice UUIDs
#define SERVICE_LED_BUTTONS_UUID   0x1000
#define CHAR_LED_CONTROL_UUID      0x1001
#define CHAR_BUTTON_STATE_UUID     0x1002

#define SERVICE_SENSORS_UUID       0x2000
#define CHAR_SENSOR_DATA_UUID      0x2001

#define SERVICE_AUDIO_UUID         0x3000
#define CHAR_VOLUME_CONTROL_UUID   0x3001
#define CHAR_SOUND_TRIGGER_UUID    0x3002

//functions
void bt_init(void);

#endif
