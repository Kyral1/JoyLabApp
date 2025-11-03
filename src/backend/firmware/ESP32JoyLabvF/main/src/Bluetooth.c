// bluetooth.c
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_common_api.h"
#include "esp_bt_main.h"

#include "LED_Control.h"   //  LED driver (led_init, led_set_color_brightness, led_clear_one)
#include "Bluetooth.h"
#include "Button_Control.h" //  button driver (button_init, button_read_all)
#include "VibrationMotor.h"
#include "Speaker_Control.h"
#include "IRS_Control.h"
#include "ForceSensor_Control.h"
#include "Game_Control.h"

#define TAG "BT_JOYLAB"

// ---- UUIDs (match your app) -------------------------------------------------
#define SRV_UUID       0x1234
#define CNTRL_UUID     0xABCD //Write without response (phone -> ESP32)
#define SETT_UUID      0xABCE //read/write (phone <-> ESP32)
#define EVNTS_UUID     0xABCF //notify (ESP32 -> phone)
#define NUM_HANDLES   12

// ---- Advertising ------------------------------------------------------------
static const char *kDeviceName = "JoyLabToy"; // tells ESP32 to advertise itself as JoyLabToy

//Categories (Cat)
typedef enum {
    CAT_LED = 0x01,
    CAT_AUDIO = 0x02,
    CAT_MOTOR = 0x03,
    CAT_GAME = 0x04,
    CAT_BUTTON = 0x05,
    CAT_IRS = 0x06,
    CAT_FORCE = 0x07,
};

//Commands - Force Sensor
enum{
  CMD_FORCE_START_CONT = 0x01, 
  CMD_FORCE_END_CONT = 0x02,
};

//Commands - IRS
enum{
  CMD_IRS_START_CONT = 0x01,
  CMD_IRS_END_CONT = 0x02,
};

//Commands (cmd) - LED
enum{
    CMD_LED_SET_PIXEL = 0x01, //payload: 
    CMD_LED_SET_MODE = 0x02,  //payload: modeID
    CMD_LED_SET_BRIGHT = 0x03, //payload: brightness 0-100
    CMD_GM_START_TRIAL = 0x10,
    CMD_GM_CANCEL      = 0x11,
};

//commands - AUDIO
enum{
    CMD_AU_SET_STATE = 0x01,
    CMD_AU_SET_VOL = 0x02,   //payload: volume 0-100
};

//commands - MOTOR
enum{
    CMD_MO_SET_STATE = 0x01, //payload: intensity 0-100
};

//commands - GAME
enum{
    CMD_GA_START = 0x01, //payload: gameID
    CMD_GA_STOP = 0x02,  //payload: none
    CMD_LED_MODE = 0x03,
    CMD_SOUND_MODE = 0x04,
    CMD_DUAL_MODE = 0x05,
};

//EVENTS (esp32 -> phone)
enum{
    EVT_GAME_RESULT = 0x81, //payload: 
};

//settings snapshot
typedef struct {
    uint8_t volume;
    uint8_t brightness;
    uint8_t LED_mode;
} joy_settings_t;

// ============================== STATE ========================================
typedef struct {
  esp_gatt_if_t ifx;            // GATT interface
  uint16_t      conn_id;        // current connection (or 0xFFFF)
  uint16_t      srv_handle;     // service handle
  uint16_t      h_ctrl;         // characteristic handles
  uint16_t      h_sett;
  uint16_t      h_evts;
  joy_settings_t settings;
  bool          led_ready;      // init LED driver once
  bool          motor_ready;
  bool          speaker_ready;
  bool          irs_ready;
  bool          force_ready;
} ble_ctx_t; //holds all runtime context

static ble_ctx_t s = {
  .ifx = ESP_GATT_IF_NONE,
  .conn_id = 0xFFFF,
  .srv_handle = 0,
  .h_ctrl = 0,
  .h_sett = 0,
  .h_evts = 0,
  .settings = { .LED_mode = 1, .volume = 60, .brightness = 100 },
  .led_ready = false,
  .motor_ready = false,
  .speaker_ready = false,
  .irs_ready = false,
  .force_ready = false,
};

static TaskHandle_t irs_task_handle = NULL;
static TaskHandle_t force_task_handle = NULL;

// advertise the 16-bit service UUID 
/*static uint8_t kSrvUuid16[2] = {
  (uint8_t)(SRV_UUID & 0xFF),
  (uint8_t)((SRV_UUID >> 8) & 0xFF),
};*/
static uint8_t kSrvUuid128[16] = {
  0xfb, 0x34, 0x9b, 0x5f,
  0x80, 0x00, 0x00, 0x80,
  0x00, 0x10, 0x00, 0x00,
  0x34, 0x12, 0x00, 0x00
};

static esp_ble_adv_params_t kAdvParams = {
    .adv_int_min       = 0x20,
    .adv_int_max       = 0x40,
    .adv_type          = ADV_TYPE_IND,
    .own_addr_type     = BLE_ADDR_TYPE_PUBLIC,
    .channel_map       = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static esp_ble_adv_data_t kAdvData = {
    .set_scan_rsp      = false,
    .include_name      = true,
    .include_txpower   = false,
    .min_interval      = 0x20,
    .max_interval      = 0x40,
    .appearance        = 0x00,
    .flag              = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
    .service_uuid_len  = sizeof(kSrvUuid128),
    .p_service_uuid    = kSrvUuid128,
};

// =========================== SMALL HELPERS ===================================
static inline void start_advertising(void) {
    esp_ble_gap_start_advertising(&kAdvParams);
    ESP_LOGI(TAG, "Started advertising");
}

static void ensure_led_ready(void){
    if (!s.led_ready) {led_init(); s.led_ready = true;}
}

static void ensure_motor_ready(void){
  if(!s.motor_ready){vibration_init(); s.motor_ready = true;}
}

static void ensure_speaker_ready(void){
  if(!s.speaker_ready){speaker_init(); s.speaker_ready = true;}
}

static void ensure_irs_ready(void){
  if(!s.irs_ready){irs_init(); s.irs_ready = true;}
}

static void ensure_force_ready(void){
  if(!s.force_ready){irs_init(); s.force_ready = true;}
}
// Common wrapper to add a characteristic and return its handle later in ADD_CHAR_EVT.
static esp_err_t add_char16(uint16_t uuid16,
                            uint16_t perms,
                            uint8_t  props,
                            const uint8_t *init_val, uint16_t init_len, uint16_t max_len) {
  esp_bt_uuid_t u = { .len = ESP_UUID_LEN_16, .uuid = { .uuid16 = uuid16 } };
  esp_attr_value_t v = {
    .attr_max_len = max_len,
    .attr_len     = init_len,
    .attr_value   = (uint8_t*)init_val,
  };
  return esp_ble_gatts_add_char(s.srv_handle, &u, perms, props, &v, NULL);
}

// ========================== SETTINGS (R/W) ===================================
static void settings_send_read_rsp(esp_ble_gatts_cb_param_t *p) {
  // Respond with current settings (3 bytes)
  esp_gatt_rsp_t rsp = {0};
  rsp.attr_value.handle = p->read.handle;
  rsp.attr_value.len    = 3;
  rsp.attr_value.value[0] = s.settings.LED_mode;
  rsp.attr_value.value[1] = s.settings.volume;
  rsp.attr_value.value[2] = s.settings.brightness;
  esp_ble_gatts_send_response(s.ifx, p->read.conn_id, p->read.trans_id, ESP_GATT_OK, &rsp);
}

static void settings_apply_and_store(const uint8_t *val, uint16_t len) {
  if (len < 3) return; // ignore malformed
  s.settings.LED_mode   = val[0];
  s.settings.volume     = val[1];
  s.settings.brightness = val[2];
  // Keep GATT attribute value in sync with our struct (so later reads reflect updates)
  esp_ble_gatts_set_attr_value(s.h_sett, 3, (uint8_t*)&s.settings);
  ESP_LOGI(TAG, "Settings updated: mode=%u vol=%u bright=%u",
           s.settings.LED_mode, s.settings.volume, s.settings.brightness);
  // TODO: apply to your subsystems (PWM for volume, default LED brightness, etc.)
}

// ========================== EVENTS (Notify) ==================================

static void evt_notify_game_result_ms(uint16_t ms) {
  if (!s.h_evts || s.conn_id == 0xFFFF) return;
  uint8_t payload[3] = { EVT_GAME_RESULT, (uint8_t)(ms & 0xFF), (uint8_t)(ms >> 8) };
  esp_ble_gatts_send_indicate(s.ifx, s.conn_id, s.h_evts, sizeof(payload), payload, false);
}

//BLE Notify helpers
//IRS Distance notify:
static void ble_notify_distance(uint16_t distance_mm) {
  if (!s.h_evts || s.conn_id == 0xFFFF) return;
    // Event payload: [event_code, low_byte, high_byte]
    uint8_t payload[3];
    payload[0] = 0x90;                       // custom event code for IRS
    payload[1] = distance_mm & 0xFF;         // lower byte
    payload[2] = (distance_mm >> 8) & 0xFF;  // upper byte
    esp_ble_gatts_send_indicate(
        s.ifx,
        s.conn_id,
        s.h_evts,
        sizeof(payload),
        payload,
        false  // notification, not indication
    );
}

//sending pressure data helper
static void ble_notify_force(float pressure){
  if (!s.h_evts || s.conn_id == 0xFFFF) return;
  uint16_t scaled = (uint16_t)(pressure * 100); //sends PSI
  uint8_t payload[3];
  payload[0] = 0x91;
  payload[1] = scaled & 0xFF;
  payload[2] = (scaled >> 8) & 0xFF;
      esp_ble_gatts_send_indicate(
        s.ifx,
        s.conn_id,
        s.h_evts,
        sizeof(payload),
        payload,
        false  // notification, not indication
    );
}
// ============================ CONTROL (WriteNR) ==============================

static void cmd_led_set_pixel(uint8_t idx, uint8_t r, uint8_t g, uint8_t b, uint8_t br_pct) {
  ensure_led_ready();
  float br = (float)br_pct / 100.0f;
  if(idx == 111){
    for(int i = 0; i<72; i++){
      led_set_color_brightness(i, r, g, b, br);
    }
  }else{
      led_set_color_brightness(idx, r, g, b, br);
  }
  led_show();
}

static void cmd_led_set_mode(uint8_t mode) {
  s.settings.LED_mode = mode;
  ESP_LOGI(TAG, "LED mode set to %u", mode);
  // TODO: wire into your game/mode engine
}

static void cmd_audio_set_state(uint8_t on){
  ensure_speaker_ready();  // ensure pin is ready
  bool state = (on > 0);
  speaker_set_state(state);
  ESP_LOGI(TAG, "BLE Speaker toggle: %s", state ? "ON" : "OFF");
}

static void cmd_audio_set_volume(uint8_t vol) {
    ensure_speaker_ready(); 
    speaker_set_volume(vol);
    ESP_LOGI(TAG, "BLE Speaker volume: %d%%", vol);
}

static void cmd_motor_set_state(uint8_t on) {
  ensure_motor_ready();
  bool state = (on >0);
  vibration_set_state(state);
  ESP_LOGI(TAG, "BLE Motor toggle: %s", state ? "ON" : "OFF");
}

static void irs_continuous_task(void *pvParameters) {
    ensure_irs_ready();  
    while (1) {
        uint16_t distance = irs_read_distance_mm();
        ble_notify_distance(distance);
    }
}

static void force_continuous_task(void *pvParameters){
  ensure_force_ready();
  while(1){
    float pressure = force_sensor_read();
    ble_notify_force(pressure);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

static void start_irs_continuous_task(void) {
    if (irs_task_handle == NULL) {
        ESP_LOGI(TAG, "Starting IRS continuous updates...");
        xTaskCreate(
            irs_continuous_task,    // Task function
            "irs_continuous_task",  // Task name
            4096,                   // Stack size
            NULL,                   // Parameters
            5,                      // Priority
            &irs_task_handle        // Task handle
        );
    } else {
        ESP_LOGW(TAG, "IRS continuous task already running");
    }
}

static void start_force_continuous_task(void){
  if(force_task_handle == NULL){
    xTaskCreate(force_continuous_task, 
      "force_task", 
      4096, 
      NULL, 
      5, 
      &force_task_handle);
  }else{
    ESP_LOGW(TAG, "Force task already running");
  }
}

// Stop continuous ranging + BLE updates
static void stop_irs_continuous_task(void) {
    if (irs_task_handle != NULL) {
        ESP_LOGI(TAG, "Stopping IRS continuous updates...");
        vTaskDelete(irs_task_handle);
        irs_task_handle = NULL;
    } else {
        ESP_LOGW(TAG, "IRS continuous task not running");
    }
}

static void stop_force_continuous_task(void){
  if(force_task_handle != NULL){
    ESP_LOGI(TAG, "Stopping Force Sensor updates...");
    vTaskDelete(force_task_handle);
    force_task_handle = NULL;
  }else{
    ESP_LOGW(TAG, "Force task not running");
  }
}

static void cmd_game_start_trial(void) {
  ESP_LOGI(TAG, "Start reaction trial");
  // TODO: arm sensors, record t0, choose LED target based on led_mode, etc.
  // For demo: pretend result after 250 ms (call notify when actual result ready)
  // evt_notify_game_result_ms(250);
}

static void cmd_game_cancel(void) {
  ESP_LOGI(TAG, "Cancel trial");
  // TODO: stop timers, clear LEDs, disarm sensors
}

static void ctrl_handle_frame(const uint8_t *buf, uint16_t n) {
  if (n < 3) return;
  uint8_t cat = buf[0], cmd = buf[1], len = buf[2];
  if (n < (uint16_t)(3 + len)) return;     // malformed
  const uint8_t *pl = buf + 3;

  switch (cat) {
    case CAT_LED:
      switch (cmd) {
        case CMD_LED_SET_PIXEL:
          if (len >= 5) cmd_led_set_pixel(pl[0], pl[1], pl[2], pl[3], pl[4]);
          break;
        case CMD_LED_SET_MODE:
          if (len >= 1) cmd_led_set_mode(pl[0]);
          break;
        case CMD_LED_SET_BRIGHT:
          if (len>=1){
            float brightness = (float)pl[0] / 100.0f;
            ensure_led_ready();
            led_set_global_brightness(brightness);
          } break;
        default: break;
      }
      break;

    case CAT_AUDIO:
      switch (cmd){
        case CMD_AU_SET_STATE:
          if (len >= 1) cmd_audio_set_state(pl[0]);
          break;
        case CMD_AU_SET_VOL:
          if (len >= 1) cmd_audio_set_volume(pl[0]);
          break;
      }
      break;

    case CAT_MOTOR:
      if (cmd == CMD_MO_SET_STATE && len >= 1) cmd_motor_set_state(pl[0]);
      break;

    case CAT_GAME:
      if (cmd == CMD_GM_START_TRIAL) cmd_game_start_trial();
      else if (cmd == CMD_GM_CANCEL)  cmd_game_cancel();
      break;
    
    case CAT_IRS:
      switch (cmd) {
        case CMD_IRS_START_CONT:
            start_irs_continuous_task();
            break;

        case CMD_IRS_END_CONT:
            stop_irs_continuous_task();
            break;
      }
      break;
    
    case CAT_FORCE:
      switch (cmd) {
        case CMD_FORCE_START_CONT:
          start_force_continuous_task();
          break;
        case CMD_FORCE_END_CONT:
          stop_force_continuous_task();
          break;
      }break;
    
    case CAT_GAME:
      switch (cmd) {
        case CMD_LED_MODE:
          start_whackamole_game();
          break;
        case CMD_GA_STOP:
          stop_whackamole_game();
          break;
      }break;

    default:
      // Unknown category — safely ignore
      break;
  }
}


// ============================= GAP CALLBACK ==================================

static void gap_cb(esp_gap_ble_cb_event_t e, esp_ble_gap_cb_param_t *p) {
  switch (e) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
      start_advertising();
      break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
      if (p->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS) ESP_LOGI(TAG, "Advertising");
      else ESP_LOGE(TAG, "Adv start failed: %d", p->adv_start_cmpl.status);
      break;
    default: break;
  }
}

// ============================= GATTS CALLBACK =================================

static void gatts_cb(esp_gatts_cb_event_t e, esp_gatt_if_t ifx, esp_ble_gatts_cb_param_t *p) {
  switch (e) {
    case ESP_GATTS_REG_EVT: {
      s.ifx = ifx;
      esp_ble_gap_set_device_name(kDeviceName);
      esp_ble_gap_config_adv_data(&kAdvData);

      esp_gatt_srvc_id_t sid = {
        .is_primary = true,
        .id = { .inst_id = 0, .uuid = { .len = ESP_UUID_LEN_16, .uuid = { .uuid16 = SRV_UUID } } }
      };
      esp_ble_gatts_create_service(ifx, &sid, NUM_HANDLES);
      break;
    }

    case ESP_GATTS_CREATE_EVT: {
      s.srv_handle = p->create.service_handle;
      esp_ble_gatts_start_service(s.srv_handle);

      // CONTROL (Write NR)
      uint8_t init_ctrl = 0x00;
      ESP_ERROR_CHECK(add_char16(
        CTRL_UUID,
        ESP_GATT_PERM_WRITE,
        ESP_GATT_CHAR_PROP_BIT_WRITE_NR,
        &init_ctrl, 1, 64
      ));

      // SETTINGS (Read/Write) — initial snapshot is 3 bytes
      uint8_t init_sett[3] = { s.settings.LED_mode, s.settings.volume, s.settings.brightness };
      ESP_ERROR_CHECK(add_char16(
        SETT_UUID,
        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
        ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE,
        init_sett, sizeof(init_sett), 16
      ));

      // EVENTS (Notify)
      uint8_t init_evt = 0x00;
      ESP_ERROR_CHECK(add_char16(
        EVTS_UUID,
        ESP_GATT_PERM_READ,
        ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
        &init_evt, 1, 20
      ));
      break;
    }

    case ESP_GATTS_ADD_CHAR_EVT: {
      uint16_t u = p->add_char.char_uuid.uuid.uuid16;
      if (u == CTRL_UUID) { s.h_ctrl = p->add_char.attr_handle; ESP_LOGI(TAG,"CTRL h=0x%04X", s.h_ctrl); }
      else if (u == SETT_UUID) { s.h_sett = p->add_char.attr_handle; ESP_LOGI(TAG,"SETT h=0x%04X", s.h_sett); }
      else if (u == EVTS_UUID) { s.h_evts = p->add_char.attr_handle; ESP_LOGI(TAG,"EVTS h=0x%04X", s.h_evts); }
      break;
    }

    case ESP_GATTS_READ_EVT:
      if (p->read.handle == s.h_sett) settings_send_read_rsp(p);
      else {
        // default OK empty
        esp_gatt_rsp_t rsp = {0};
        rsp.attr_value.handle = p->read.handle;
        rsp.attr_value.len    = 0;
        esp_ble_gatts_send_response(ifx, p->read.conn_id, p->read.trans_id, ESP_GATT_OK, &rsp);
      }
      break;

    case ESP_GATTS_WRITE_EVT:
      if (p->write.handle == s.h_ctrl) {
        ctrl_handle_frame(p->write.value, p->write.len);
      } else if (p->write.handle == s.h_sett) {
        settings_apply_and_store(p->write.value, p->write.len);
      } else {
        // writes to CCCD or others — ignore
      }
      break;

    case ESP_GATTS_CONNECT_EVT:
      s.conn_id = p->connect.conn_id;
      ESP_LOGI(TAG, "Connected (conn=%u)", s.conn_id);
      break;

    case ESP_GATTS_DISCONNECT_EVT:
      ESP_LOGI(TAG, "Disconnected");
      s.conn_id = 0xFFFF;
      start_advertising();
      break;

    default: break;
  }
}

// ============================== PUBLIC API ===================================

esp_err_t bluetooth_init(void) {
  esp_err_t err;

  // NVS for BT stack
  err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  // BLE only
  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
  ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
  ESP_ERROR_CHECK(esp_bluedroid_init());
  ESP_ERROR_CHECK(esp_bluedroid_enable());

  ESP_ERROR_CHECK(esp_ble_gap_register_callback(gap_cb));
  ESP_ERROR_CHECK(esp_ble_gatts_register_callback(gatts_cb));
  ESP_ERROR_CHECK(esp_ble_gatts_app_register(0x55));  // single app profile

  ESP_LOGI(TAG, "BLE init done");
  return ESP_OK;
}

void bluetooth_deinit(void) {
  esp_ble_gap_stop_advertising();
  if (s.srv_handle) esp_ble_gatts_stop_service(s.srv_handle);
  esp_bluedroid_disable();
  esp_bluedroid_deinit();
  esp_bt_controller_disable();
  esp_bt_controller_deinit();
  ESP_LOGI(TAG, "BLE deinit done");
}