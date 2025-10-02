#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gatt_common_api.h"

#include "bluetooth.h"

//log tag - to identify BLE related messages
static const char *TAG = "JOYLAB_BLE";

static esp_ble_adv_data_t adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40, //interval range between 20-40ms
    .adv_type           = ADV_TYPE_IND, //any device can connect
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC, //use public addr
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

//GATT Database Indexes
enum { //indexes into attribute table
    IDX_SVC_LED_BUTTONS,
    IDX_CHAR_LED_CONTROL,
    IDX_CHAR_LED_CONTROL_VAL,
    IDX_CHAR_BUTTON_STATE,
    IDX_CHAR_BUTTON_STATE_VAL,

    IDX_SVC_SENSORS,
    IDX_CHAR_SENSOR_DATA,
    IDX_CHAR_SENSOR_DATA_VAL,

    IDX_SVC_AUDIO,
    IDX_CHAR_VOLUME_CONTROL,
    IDX_CHAR_VOLUME_CONTROL_VAL,
    IDX_CHAR_SOUND_TRIGGER,
    IDX_CHAR_SOUND_TRIGGER_VAL,

    HRS_IDX_NB,   //total number of entries in DB
};

//initializes bluetooth
void bt_init(void){
    ESP_LOGI(TAG, "Bluetooth initialized (stub)."); //prints message to alog
}
