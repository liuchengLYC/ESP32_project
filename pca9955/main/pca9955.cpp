#include "pca9955.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static uint32_t i2c_freq = 100000;
static uint8_t iref_current = 0xFF;
static int timeout = 50;

PCA9955::PCA9955(i2c_master_bus_handle_t& bus_handle, uint8_t chip_addr){
    ESP_ERROR_CHECK(init(bus_handle, chip_addr));
}

esp_err_t PCA9955::init(i2c_master_bus_handle_t& bus_handle, uint8_t chip_addr){
    i2c_device_config_t dev_cfg = {
        .scl_speed_hz = i2c_freq,
        .device_address = chip_addr,
    };
    esp_err_t ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);
    if(ret != ESP_OK) return ret;
    uint8_t buf[2] = {0x00, 0x05};  //listen to all call and subcall
    ret = i2c_master_transmit(dev_handle, buf, 2, timeout);
    if(ret != ESP_OK) return ret;
    buf[0] = 0x45, buf[1] = iref_current; 
    ret = i2c_master_transmit(dev_handle, buf, 2, timeout);
    return ret;
}

esp_err_t PCA9955::write(uint8_t data[][3],int len, int r, int g, int b){
    uint8_t reg_r = r + 8, reg_g = g + 8, reg_b = b + 8; 
    for(int i = 0; i < len; i++){
        uint8_t bufr[2] = {reg_r, data[i][0]};
        uint8_t bufg[2] = {reg_g, data[i][1]};
        uint8_t bufb[2] = {reg_b, data[i][2]};
        esp_err_t ret;
        ret = i2c_master_transmit(dev_handle, bufr, 2, timeout);
        ret = i2c_master_transmit(dev_handle, bufg, 2, timeout);
        ret = i2c_master_transmit(dev_handle, bufb, 2, timeout);
        vTaskdelay(pdMS_TO_TICKS(50));
        if(ret != ESP_OK) return ret;
    }
    return ESP_OK;
}