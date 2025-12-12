// bluetooth.c
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_gatt_common_api.h"
#include "Bluetooth.h"
#include "IMU_Control.h"
#include "Motion_task.h"

#define TAG "BT_JLWristBand"

// ---- UUIDs -------------------------------------------------
#define SRV_UUID       0x2234
#define CNTRL_UUID     0xBBCD  // Write without response (phone -> ESP32)
#define EVNTS_UUID     0xBBCE  // Notify (ESP32 -> phone)
#define NUM_HANDLES    8

// ---- Advertising ------------------------------------------------------------
static const char *kDeviceName = "JoyLabWristband";

//Categories (Cat)
typedef enum {
    CAT_ACCEL = 0x01,
};

//Accelerometer Commands
enum{
  CMD_MOTION_START = 0x01,
  CMD_MOTION_STOP = 0x02,
};

// ============================== STATE ========================================
typedef struct {
  esp_gatt_if_t ifx;
  uint16_t conn_id;
  uint16_t srv_handle;
  uint16_t h_ctrl;
  uint16_t h_evts;
  bool imu_ready;
} ble_ctx_t;

static ble_ctx_t s = {
  .ifx = ESP_GATT_IF_NONE,
  .conn_id = 0xFFFF,
  .srv_handle = 0,
  .h_ctrl = 0,
  .h_evts = 0,
};

// ============================= Advertising ====================================
static uint8_t kSrvUuid128[16] = {
  0xfb, 0x34, 0x9b, 0x5f,
  0x80, 0x00, 0x00, 0x80,
  0x00, 0x10, 0x00, 0x00,
  0x34, 0x22, 0x00, 0x00
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

void ensure_BMI_ready(void){
    if(!s.imu_ready){imu_init(); s.imu_ready = true;}
}

static esp_err_t add_char16(uint16_t uuid16, uint16_t perms, uint8_t props,
                            const uint8_t *init_val, uint16_t init_len, uint16_t max_len) {
  esp_bt_uuid_t u = { .len = ESP_UUID_LEN_16, .uuid = { .uuid16 = uuid16 } };
  esp_attr_value_t v = {
    .attr_max_len = max_len,
    .attr_len     = init_len,
    .attr_value   = (uint8_t*)init_val,
  };
  return esp_ble_gatts_add_char(s.srv_handle, &u, perms, props, &v, NULL);
}

// =========================== Notify IMU Data ============================
void ble_notify_imu(float gx, float gy, float gz) {
  if (!s.h_evts || s.conn_id == 0xFFFF) return;
  uint8_t payload[12];
  memcpy(payload, &gx, 4);
  memcpy(payload + 4, &gy, 4);
  memcpy(payload + 8, &gz, 4);
  esp_ble_gatts_send_indicate(s.ifx, s.conn_id, s.h_evts, sizeof(payload), payload, false);
}

// ========================== EVENTS (Notify) ==================================

void evt_notify_motion_detection(uint8_t motion_flags){
  if (!s.h_evts || s.conn_id == 0xFFFF) return;

    // Payload format: [event_code, points, attempts]
    uint8_t payload[2];
    payload[0] = 0x87;
    payload[1] = motion_flags;

    esp_ble_gatts_send_indicate(
        s.ifx,
        s.conn_id,
        s.h_evts,
        sizeof(payload),
        payload,
        false  // notification (not indication)
    );

    ESP_LOGI(TAG, "Sent Dual Mode Results");
}

// ============================ Control Handler ===========================
/*static void ctrl_handle_frame(const uint8_t *buf, uint16_t n) {
  // You can add simple commands here like start/stop IMU streaming
  ESP_LOGI(TAG, "BLE command received, len=%d", n);
}*/
static void ctrl_handle_frame(const uint8_t *buf, uint16_t n) {
  if (n < 3) return;
  uint8_t cat = buf[0], cmd = buf[1], len = buf[2];
  if (n < (uint16_t)(3 + len)) return;     // malformed
  const uint8_t *pl = buf + 3;

  switch (cat) {
    case CAT_ACCEL:
      switch (cmd) {
        case CMD_MOTION_START:
          start_motion_detection();
          break;
        case CMD_MOTION_STOP:
          stop_motion_detection();
          break;
      }
      break;
    default:
      // Unknown category — safely ignore
    break;
  }
}

// ============================= GAP CALLBACK =============================
static void gap_cb(esp_gap_ble_cb_event_t e, esp_ble_gap_cb_param_t *p) {
  if (e == ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT) start_advertising();
}

// ============================= GATTS CALLBACK ===========================
static void gatts_cb(esp_gatts_cb_event_t e, esp_gatt_if_t ifx, esp_ble_gatts_cb_param_t *p) {
  switch (e) {
    case ESP_GATTS_REG_EVT:
      s.ifx = ifx;
      esp_ble_gap_set_device_name(kDeviceName);
      esp_ble_gap_config_adv_data(&kAdvData);
      esp_gatt_srvc_id_t sid = {
        .is_primary = true,
        .id = { .inst_id = 0, .uuid = { .len = ESP_UUID_LEN_16, .uuid = { .uuid16 = SRV_UUID } } }
      };
      esp_ble_gatts_create_service(ifx, &sid, NUM_HANDLES);
      break;

    case ESP_GATTS_CREATE_EVT:
      s.srv_handle = p->create.service_handle;
      esp_ble_gatts_start_service(s.srv_handle);

      uint8_t init_ctrl = 0x00;
      ESP_ERROR_CHECK(add_char16(CNTRL_UUID, ESP_GATT_PERM_WRITE,
                                 ESP_GATT_CHAR_PROP_BIT_WRITE_NR,
                                 &init_ctrl, 1, 64));

      uint8_t init_evt = 0x00;
      ESP_ERROR_CHECK(add_char16(EVNTS_UUID, ESP_GATT_PERM_READ,
                                 ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY,
                                 &init_evt, 1, 20));
      break;

    case ESP_GATTS_ADD_CHAR_EVT:
      if (p->add_char.char_uuid.uuid.uuid16 == CNTRL_UUID)
        s.h_ctrl = p->add_char.attr_handle;
      else if (p->add_char.char_uuid.uuid.uuid16 == EVNTS_UUID)
        s.h_evts = p->add_char.attr_handle;
      break;

    case ESP_GATTS_WRITE_EVT:
      if (p->write.handle == s.h_ctrl)
        ctrl_handle_frame(p->write.value, p->write.len);
      break;

    case ESP_GATTS_CONNECT_EVT:
      s.conn_id = p->connect.conn_id;
      ESP_LOGI(TAG, "Connected");
      break;

    case ESP_GATTS_DISCONNECT_EVT:
      s.conn_id = 0xFFFF;
      start_advertising();
      break;

    default: break;
  }
}

// ============================== PUBLIC API ==============================
esp_err_t bluetooth_init(void) {
  nvs_flash_init();
  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  esp_bt_controller_init(&bt_cfg);
  esp_bt_controller_enable(ESP_BT_MODE_BLE);
  esp_bluedroid_init();
  esp_bluedroid_enable();
  esp_ble_gap_register_callback(gap_cb);
  esp_ble_gatts_register_callback(gatts_cb);
  esp_ble_gatts_app_register(0x55);
  ESP_LOGI(TAG, "BLE initialized");
  return ESP_OK;
}
