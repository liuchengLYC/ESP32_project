#include "pca9955.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
i2c_master_bus_handle_t bus_handle;
const char* tag = "test";

extern "C" void app_main(){
    auto start = esp_timer_get_time();
    ESP_ERROR_CHECK(bus_init(bus_handle, 0, GPIO_NUM_21, GPIO_NUM_22));
    auto end = esp_timer_get_time();
    ESP_LOGI(tag, "bus init uses %lldus", end - start);
    start = esp_timer_get_time();
    PCA9955 device(bus_handle, 0x22, 5);
    end = esp_timer_get_time();
    ESP_LOGI(tag, "device init uses %lldus", end - start);
    uint8_t data[3] = {255, 255, 255};
    start = esp_timer_get_time();
    ESP_ERROR_CHECK(device.display_color(data, 0));
    end = esp_timer_get_time();
    ESP_LOGI(tag, "color display uses %lldus", end - start);
}