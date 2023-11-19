#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "heartbeat.h"
#include "wifi_server_task.h"
#include "wifi_server.h"
#include "timer_group_isr.h"
#include "mutex_handler.h"
#include "esp_log.h"

static const char CHIP_TAG[] = "CPU_CORE";

void chip_info(void)
{
    /* Prints chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    ESP_LOGI(CHIP_TAG, "This is %s chip with %d CPU core(s), WiFi%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;

    ESP_LOGI(CHIP_TAG, "silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        ESP_LOGW(CHIP_TAG, "Get flash size failed");
        return;
    }

    ESP_LOGI(CHIP_TAG, "%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    ESP_LOGI(CHIP_TAG, "Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    fflush(stdout);
}
void app_main(void)
{
    /*Print Chip Info*/
    chip_info();
    
    vCreateMutex();
    /*Start Heartbeat Task*/
    int heartbeat_code = start_heartbeat_task();

    int wifi_code = start_wifi_task();
    // start_rest_get();
    start_timer_isr_task();
    
}
