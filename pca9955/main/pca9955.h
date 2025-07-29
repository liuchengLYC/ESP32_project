#pragma once
#include "driver/i2c_master.h"
#include "esp_err.h"

class PCA9955{
    public:
        PCA9955(i2c_master_bus_handle_t& bus_handle, uint8_t chip_addr);
        esp_err_t write(uint8_t data[][3], int len, int r, int g, int b); //rgb attach to which pin

    private:
        i2c_master_dev_handle_t dev_handle;
        esp_err_t init(i2c_master_bus_handle_t& bus_handle, uint8_t chip_addr);
};