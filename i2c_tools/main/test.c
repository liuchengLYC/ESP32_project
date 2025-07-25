#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_fat.h"
#include "driver/i2c_master.h"
#include "func_i2ctools.h"

static gpio_num_t i2c_gpio_sda = CONFIG_EXAMPLE_I2C_MASTER_SDA;
static gpio_num_t i2c_gpio_scl = CONFIG_EXAMPLE_I2C_MASTER_SCL;

static i2c_port_t i2c_port = I2C_NUM_0;
i2c_master_bus_handle_t handle = NULL;

void app_main(){
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = i2c_port,
        .scl_io_num = i2c_gpio_scl,
        .sda_io_num = i2c_gpio_sda,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    uint8_t data = 0x55;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &tool_bus_handle));
    //i2c_scan();
    ESP_ERROR_CHECK(i2c_write(0x22, 0x02, &data, 1));
    data = 0xFF;
    //ESP_ERROR_CHECK(i2c_write(0x22, 0x44, &data, 1));
    ESP_ERROR_CHECK(i2c_write(0x22, 0x45, &data, 1));
    for(uint8_t i = 0x00; i < 0xFF; i++){
        data = i;
        i2c_write(0x22, 0x44, &data, 1);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}