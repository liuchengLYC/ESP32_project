#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define FRAME_RATE_HZ 5                
#define INITIAL_X 1
#define INITIAL_Y 1
#define VELOCITY_X 1                   
#define VELOCITY_Y 1                   
#define END_X 6                   
#define END_Y 6               

#define PIN_NUM_MOSI 26
#define PIN_NUM_CLK 32
#define PIN_NUM_CS 33
#define DISPLAY_SIZE 8

static spi_device_handle_t spi;
static int ball_x = INITIAL_X;
static int ball_y = INITIAL_Y;
static int vx = VELOCITY_X;
static int vy = VELOCITY_Y;
static int frame_count = 0;
static bool reached_end = false;

static uint8_t display_buffer[8] = {0};  // 每列一個 byte，代表 8 個 LED

// ======= MAX7219 SPI 傳輸 =======
void max7219_send(uint8_t address, uint8_t data) {
    uint8_t tx_data[2] = {address, data};
    spi_transaction_t t = {
        .length = 16, // bits
        .tx_buffer = tx_data
    };
    spi_device_transmit(spi, &t); // 阻塞式傳輸
}

void max7219_init() {
    max7219_send(0x0F, 0x00); // display test: off
    max7219_send(0x0C, 0x01); // shutdown: normal operation
    max7219_send(0x0B, 0x07); // scan limit: 8 digits
    max7219_send(0x09, 0x00); // decode mode: none
    max7219_send(0x0A, 0x08); // intensity: mid 全開好亮
    for (int i = 1; i <= 8; i++) {
        max7219_send(i, 0x00); // 清空畫面
    }
}

// ======= Timer Callback Function =======
void IRAM_ATTR timer_callback(void* arg) {
    if (reached_end) return;

    frame_count++;

    // 清空 buffer
    memset(display_buffer, 0, sizeof(display_buffer));

    // 畫左側牆壁（最左邊 bit = 1）
    for (int y = 0; y < DISPLAY_SIZE; y++) {
        display_buffer[y] |= 0x80; // 10000000
    }

    // 更新球的位置
    int next_x = ball_x + vx;
    int next_y = ball_y + vy;

    // 撞牆或邊界反彈
    if (next_x >= DISPLAY_SIZE || next_x <= 1) vx *= -1;
    if (next_y >= DISPLAY_SIZE || next_y < 0) vy *= -1;

    ball_x += vx;
    ball_y += vy;

    // 畫球（注意：最左是 bit7）
    display_buffer[ball_y] |= (1 << (7 - ball_x));

    // 傳送畫面至 MAX7219
    for (int i = 0; i < DISPLAY_SIZE; i++) {
        max7219_send(i + 1, display_buffer[i]);
    }

    // 是否到達終點？
    if (ball_x == END_X && ball_y == END_Y) {
        reached_end = true;

        // 清空畫面
        for (int i = 1; i <= DISPLAY_SIZE; i++) {
            max7219_send(i, 0x00);
        }

        // 輸出 frame 數
        printf("Total frames: %d\n", frame_count);
    }
}

// ======= 主程式入口點 =======
void app_main() {
    // SPI 總線初始化
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000, // 1 MHz
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 1 
    };

    spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(HSPI_HOST, &devcfg, &spi);

    max7219_init();

    // 建立並啟動 timer
    esp_timer_handle_t periodic_timer;
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &timer_callback,
        .name = "frame_timer",
    };
    esp_timer_create(&periodic_timer_args, &periodic_timer);
    esp_timer_start_periodic(periodic_timer, 1000000 / FRAME_RATE_HZ);

    printf("Bouncing ball started!\n");
}
