#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"

const int M1[4][4] = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
const int M2[4][4] = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
int M3[4][4] = {0};
int sum = 0;
int cur_job = 0;
static SemaphoreHandle_t mutex;
static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

void task(void *arg){
    while(true){
        int job = -1;
        if(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE){
            if(cur_job >= 32){
                xSemaphoreGive(mutex);
                break;
            }else{
                job = cur_job++;
                xSemaphoreGive(mutex);
            }
        }
        if(job < 16){
            int row = job / 4;
            int col = job % 4;
            for(int i = 0; i < 4; i++){
                M3[row][col] += M1[row][i] * M2[i][col];
            }
            printf("M3[%d][%d] is calculate on core %d, and is equals to %d\n",row, col, esp_cpu_get_core_id(), M3[row][col]);
        }else{
            int row = (job - 16) / 4;
            int col = job % 4;
            portENTER_CRITICAL(&spinlock);
            sum += M3[row][col];
            portEXIT_CRITICAL(&spinlock);
            printf("add M3[%d][%d] to sum, running on core %d, present sum is %d\n", row, col, esp_cpu_get_core_id(), sum);
            
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    printf("task done");
    vTaskDelete(NULL);
}

void app_main(void){
    mutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(task, "task A multiply", 2048, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(task, "task B multiply", 2048, NULL, 3, NULL, 1);
}
