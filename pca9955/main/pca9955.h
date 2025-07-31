#pragma once
#include "driver/i2c_master.h"
#include "esp_err.h"
struct LED_color{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class PCA9955 {
    public:
        PCA9955(i2c_master_bus_handle_t& bus_handle, uint8_t chip_addr, int channel_num);
        ~PCA9955();
        esp_err_t display_color(LED_color *data, int channel_index);
        esp_err_t display_frame(LED_color data[][1]); //length for pca9955 is 1 
        esp_err_t adjust_iref_cur(uint8_t current, int led_pin, bool all); 

    private:
        i2c_master_dev_handle_t dev_handle;
        int channel_num;
        esp_err_t dev_init(i2c_master_bus_handle_t& bus_handle, uint8_t chip_addr);
};

esp_err_t bus_init(i2c_master_bus_handle_t& bus_handle,int port, gpio_num_t sda_gpio, gpio_num_t scl_gpio);
