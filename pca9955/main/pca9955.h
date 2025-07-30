#pragma once
#include "driver/i2c_master.h"
#include "esp_err.h"

class PCA9955{
    public:
        PCA9955(i2c_master_bus_handle_t& bus_handle, uint8_t chip_addr, int channel_num);
        ~PCA9955();
        esp_err_t display_color(uint8_t data[3], int channel_index);
        esp_err_t display_frame(uint8_t data[][3], int len);  

    private:
        i2c_master_dev_handle_t dev_handle;
        int channel_num;
        esp_err_t dev_init(i2c_master_bus_handle_t& bus_handle, uint8_t chip_addr);
};

esp_err_t bus_init(i2c_master_bus_handle_t& bus_handle,int port, gpio_num_t sda_gpio, gpio_num_t scl_gpio);