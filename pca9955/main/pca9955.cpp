#include "pca9955.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static uint32_t i2c_freq = 100000;
static uint8_t iref_current = 0x10;
static int timeout = 50;

PCA9955::PCA9955(i2c_master_bus_handle_t& bus_handle, uint8_t chip_addr, int channel_num){
    assert(channel_num > 0 && channel_num <= 5);
    ESP_ERROR_CHECK(dev_init(bus_handle, chip_addr));
    this->channel_num = channel_num;
}

PCA9955::~PCA9955(){
    i2c_master_bus_rm_device(dev_handle);
}

esp_err_t PCA9955::dev_init(i2c_master_bus_handle_t& bus_handle, uint8_t chip_addr) {
    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = chip_addr;
    dev_cfg.scl_speed_hz = i2c_freq;
    dev_cfg.scl_wait_us = 0;
    dev_cfg.flags = {};

    esp_err_t ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);
    if (ret != ESP_OK) return ret;

    uint8_t buf[2] = {0x00, 0x05};  // listen to all call and subcall
    ret = i2c_master_transmit(dev_handle, buf, 2, timeout);
    if (ret != ESP_OK) return ret;

    buf[0] = 0x45;
    buf[1] = iref_current;
    return i2c_master_transmit(dev_handle, buf, 2, timeout);
}


esp_err_t PCA9955::display_color(uint8_t data[3], int channel_index){
    uint8_t reg_r = 3 * channel_index + 8, reg_g = 3 * channel_index + 9, reg_b = 3 * channel_index + 10; 
    uint8_t bufr[2] = {reg_r, data[0]};
    uint8_t bufg[2] = {reg_g, data[1]};
    uint8_t bufb[2] = {reg_b, data[2]};
    esp_err_t ret;
    ret = i2c_master_transmit(dev_handle, bufr, 2, timeout);
    ret = i2c_master_transmit(dev_handle, bufg, 2, timeout);
    ret = i2c_master_transmit(dev_handle, bufb, 2, timeout);
    return ret;
}

esp_err_t PCA9955::display_frame(uint8_t data[][3], int len){
    return ESP_OK;
}

esp_err_t PCA9955::adjust_iref_cur(uint8_t current, int led_pin, bool all){
    uint8_t buf[2] = {0xFF, current};
    if(all) buf[0] = 0x45;
    else buf[0] = 0x18 + led_pin;
    return i2c_master_transmit(dev_handle, buf, 2, timeout);
}

esp_err_t bus_init(i2c_master_bus_handle_t& bus_handle, int port, gpio_num_t sda_gpio, gpio_num_t scl_gpio) {
    i2c_master_bus_config_t bus_cfg = {};
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_cfg.sda_io_num = sda_gpio;
    bus_cfg.scl_io_num = scl_gpio;
    bus_cfg.i2c_port = port;
    bus_cfg.glitch_ignore_cnt = 7;
    bus_cfg.flags.enable_internal_pullup = true;
    return i2c_new_master_bus(&bus_cfg, &bus_handle);
}
