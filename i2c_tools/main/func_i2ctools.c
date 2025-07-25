#include "func_i2ctools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "esp_err.h"

#define I2C_TOOL_TIMEOUT_MS 50

static const char *TAG = "i2c_tools_api";
static uint32_t i2c_frequency = 100000; // Default 100kHz
i2c_master_bus_handle_t tool_bus_handle = NULL;

/**
 * @brief Configure I2C bus
 */
esp_err_t i2c_config(int port, int sda_gpio, int scl_gpio, int freq_hz)
{
    if (port < 0 || port >= I2C_NUM_MAX) {
        ESP_LOGE(TAG, "Invalid I2C port: %d", port);
        return ESP_ERR_INVALID_ARG;
    }

    if (freq_hz > 0) {
        i2c_frequency = freq_hz;
    }

    if (tool_bus_handle) {
        i2c_del_master_bus(tool_bus_handle);
        tool_bus_handle = NULL;
    }

    i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = port,
        .scl_io_num = scl_gpio,
        .sda_io_num = sda_gpio,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    return i2c_new_master_bus(&bus_cfg, &tool_bus_handle);
}

/**
 * @brief Scan for I2C devices
 */
void i2c_scan(void)
{
    printf("Scanning I2C bus...\n");
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    for (int i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j++) {
            uint8_t addr = i + j;
            esp_err_t ret = i2c_master_probe(tool_bus_handle, addr, I2C_TOOL_TIMEOUT_MS);
            if (ret == ESP_OK) {
                printf("%02x ", addr);
            } else if (ret == ESP_ERR_TIMEOUT) {
                printf("UU ");
            } else {
                printf("-- ");
            }
        }
        printf("\n");
    }
}

/**
 * @brief Read registers from I2C device
 */
esp_err_t i2c_read(uint8_t chip_addr, uint8_t reg_addr, uint8_t *data, size_t len)
{
    i2c_device_config_t dev_cfg = {
        .scl_speed_hz = i2c_frequency,
        .device_address = chip_addr,
    };
    i2c_master_dev_handle_t dev_handle;
    esp_err_t ret = i2c_master_bus_add_device(tool_bus_handle, &dev_cfg, &dev_handle);
    if (ret != ESP_OK) return ret;

    ret = i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, I2C_TOOL_TIMEOUT_MS);

    i2c_master_bus_rm_device(dev_handle);
    return ret;
}

/**
 * @brief Write registers to I2C device
 */
esp_err_t i2c_write(uint8_t chip_addr, uint8_t reg_addr, const uint8_t *data, size_t len)
{
    i2c_device_config_t dev_cfg = {
        .scl_speed_hz = i2c_frequency,
        .device_address = chip_addr,
    };
    i2c_master_dev_handle_t dev_handle;
    esp_err_t ret = i2c_master_bus_add_device(tool_bus_handle, &dev_cfg, &dev_handle);
    if (ret != ESP_OK) return ret;

    uint8_t *buf = malloc(len + 1);
    buf[0] = reg_addr;
    memcpy(&buf[1], data, len);
    ret = i2c_master_transmit(dev_handle, buf, len + 1, I2C_TOOL_TIMEOUT_MS);

    free(buf);
    i2c_master_bus_rm_device(dev_handle);
    return ret;
}

/**
 * @brief Write some bytes then read from I2C device
 */
// esp_err_t i2c_write_read(uint8_t chip_addr, const uint8_t *write_buf, size_t write_len, uint8_t *read_buf, size_t read_len)
// {
//     i2c_device_config_t dev_cfg = {
//         .scl_speed_hz = i2c_frequency,
//         .device_address = chip_addr,
//     };
//     i2c_master_dev_handle_t dev_handle;
//     esp_err_t ret = i2c_master_bus_add_device(tool_bus_handle, &dev_cfg, &dev_handle);
//     if (ret != ESP_OK) return ret;

//     ret = i2c_master_transmit_receive(dev_handle, write_buf, write_len, read_buf, read_len, I2C_TOOL_TIMEOUT_MS);

//     i2c_master_bus_rm_device(dev_handle);
//     return ret;
// }

