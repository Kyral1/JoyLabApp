#include "bluetooth.h"
#include "esp_log.h"

//log tag - to identify BLE related messages
static const char *TAG = "JOYLAB_BT";

//initializes bluetooth
void bt_init(void){
    ESP_LOGI(TAG, "Bluetooth initialized (stub)."); //prints message to alog
}
