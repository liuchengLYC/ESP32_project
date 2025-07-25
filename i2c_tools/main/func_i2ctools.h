#pragma once

#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t i2c_config(int port, int sda_gpio, int scl_gpio, int freq_hz);
void i2c_scan(void);
esp_err_t i2c_read(uint8_t chip_addr, uint8_t reg_addr, uint8_t *data, size_t len);
esp_err_t i2c_write(uint8_t chip_addr, uint8_t reg_addr, const uint8_t *data, size_t len);
//esp_err_t i2c_write_read(uint8_t chip_addr, const uint8_t *write_buf, size_t write_len, uint8_t *read_buf, size_t read_len);

extern i2c_master_bus_handle_t tool_bus_handle;

#ifdef __cplusplus
}
#endif