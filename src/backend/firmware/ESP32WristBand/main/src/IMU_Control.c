//3 axis accelerometer (tilt, shaking, motion onset/stop, gravity orientation)
//3 axis gyroscope (wrist rotations, twisting, flapping, swiping motions)

#include "IMU_Control.h"
#include "bmi323.h"
#include "bmi323_defs.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_sys.h"

static const char *TAG = "IMU";

static struct bmi3_dev imu_dev;
static volatile bool motion_flag = false;

//I2C read/write functions for BMI323
static BMI323_INTF_RET_TYPE i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr){
    uint8_t dev_addr = *(uint8_t *)intf_ptr;

    esp_err_t res =  i2c_master_write_read_device(
        I2C_NUM_0, dev_addr, &reg_addr, 1, data, len, 1000 / portTICK_PERIOD_MS 
    );
    return (res == ESP_OK) ? BMI323_INTF_RET_SUCCESS : -1;
}

static BMI323_INTF_RET_TYPE i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr){
    uint8_t dev_addr = *(uint8_t *)intf_ptr;
    uint8_t buf[1+len];
    buf[0] = reg_addr;
    memcpy(&buf[1], data, len);

    esp_err_t res = i2c_master_write_to_device(
        I2C_NUM_0, dev_addr, buf, sizeof(buf), 1000 / portTICK_PERIOD_MS
    );

    return (res == ESP_OK) ? BMI323_INTF_RET_SUCCESS : -1;
}

static void bmi_delay_us(uint32_t us, void *intf_ptr){
    esp_rom_delay_us(us);
}

//interrupt handler 
static void IRAM_ATTR imu_handle_interrupt(void *arg){
    uint16_t int_status = 0;
    bmi323_get_int1_status(&int_status, &imu_dev);
    if(int_status & BMI3_INT_STATUS_ANY_MOTION){
        motion_flag = true;
    }
}

//initializes and prepares IMU for what data we want to collect 
void imu_init(void){
    ESP_LOGI(TAG, "Initializing I2C....");
    //I2C setup
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER, 
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = true,
        .scl_pullup_en = true,
        .master.clk_speed = 400000
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0));

    //driver struct 
    static uint8_t i2c_addr = 0x68;
    imu_dev.intf = BMI3_I2C_INTF;
    imu_dev.read = i2c_read;
    imu_dev.write = i2c_write;
    imu_dev.intf_ptr = &i2c_addr;
    imu_dev.delay_us = bmi_delay_us;

    //initialize BMI chip 
    int8_t rslt = bmi323_init(&imu_dev);
    if (rslt != BMI323_OK){
        ESP_LOGE(TAG, "BMI323 init failed: %d", rslt);
        return;
    }
    ESP_LOGI(TAG, "BMI323 Initialized.");

    //accelerometer
    struct bmi3_sens_config cfg;
    cfg.type = BMI323_ACCEL;
    cfg.cfg.acc.odr      = BMI3_ACC_ODR_100HZ;   // REQUIRED
    //cfg.cfg.acc.bwp      = BMI3_ACC_BW_NORMAL;   // REQUIRED
    cfg.cfg.acc.acc_mode = BMI3_ACC_MODE_NORMAL;
    cfg.cfg.acc.range    = BMI3_ACC_RANGE_4G;    // REQUIRED
    cfg.cfg.acc.avg_num  = 1;
    bmi323_set_sensor_config(&cfg, 1, &imu_dev);

    //motion features 
    cfg.type = BMI323_ANY_MOTION;
    cfg.cfg.any_motion.slope_thres = 10; //adjust sensitivity
    cfg.cfg.any_motion.duration = 2; 
    cfg.cfg.any_motion.acc_ref_up = 0;
    bmi3_set_sensor_config(&cfg, 1, &imu_dev);

    //orientation sensor
    cfg.type = BMI323_ORIENTATION;
    bmi323_set_sensor_config(&cfg, 1, &imu_dev);

    //map interrupt to INT1 pin
    struct bmi3_int_pin_config int_cfg = {0};
    int_cfg.pin_type = BMI3_INT1;
    int_cfg.int_latch = BMI3_INT_LATCH_EN;
    //int_cfg.output_enable = BMI3_INT_OUTPUT_ENABLE;
    bmi323_set_int_pin_config(&int_cfg, &imu_dev);

    //interrupt setup ESP32
    gpio_config_t io_config = {
        .intr_type = GPIO_INTR_NEGEDGE, //pulls low when interupt occurs
        .pin_bit_mask = 1ULL << 17, 
        .mode = GPIO_MODE_INPUT, 
        .pull_up_en = 1
    };
    gpio_config(&io_config);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(17, imu_handle_interrupt, NULL);
    ESP_LOGI(TAG, "IMU interrupt enabled.");
}


//getter for the motion flag 
bool imu_motion_detected(void){
    bool temp = motion_flag;
    motion_flag = false; //reset
    return temp;
}

//getting orientation 
int imu_get_orientation(void){
    uint8_t orient = 0;
    esp_err_t res;

    const uint8_t ORIENT_REG = 0x1E;

    res = i2c_master_write_read_device(
        I2C_NUM_0, 
        0x68,
        &ORIENT_REG, 
        1,
        &orient, 
        1,
        1000/portTICK_PERIOD_MS
    );
    if (res != ESP_OK){
        ESP_LOGI(TAG, "Failed to read orientation reg");
        return -1;
    }
    return orient;
}
