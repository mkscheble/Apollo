// Includes
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_server_task.h"
#include "wifi_server.h"
#include <time.h>
#include "esp_timer.h"
#include "mutex_handler.h"
#include "esp_log.h"

// Start connection with wifi to obtain sun information
void wifiTask()
{   
    start_rest_get();

    while (1) {

        if(xMutexPermission != NULL)
        {
            // See if we can obtain the mutex.  If the mutex is unavailable wait 20 ticks to see if it comes free
            if(xSemaphoreTake(xMutexPermission, (TickType_t) 20) == pdTRUE)
            {
                // We were able to access mutex.
                if (permission_status) {
                    ESP_LOGI(WIFI_TAG, "Restful API Call");
                    rest_get();
                    permission_status = false;
                    vTaskDelay(1000 / portTICK_PERIOD_MS);

                } 
                // Release the Mutex
                xSemaphoreGive(xMutexPermission);
            }
            else
            {
                ESP_LOGW(WIFI_TAG, "Mutex locked. Unable to access wifi variable safely");
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


int start_wifi_task(void)
{
    {
    TaskHandle_t wifi_handle = NULL;
    if (xTaskCreate(wifiTask, "Wifi Task", 4096, NULL, tskIDLE_PRIORITY + 2, &wifi_handle) != pdPASS)
    {   
        ESP_LOGI(WIFI_TAG, "Wifi Task creation failed! reboot needed.");
        for (int i = 10; i >= 0; i--) {
            ESP_LOGI(WIFI_TAG, "Restarting in %d seconds...", i);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
        ESP_LOGI(WIFI_TAG, "Restarting now.");
        fflush(stdout);
        esp_restart();
        return 0;
    }
    return 1;
}
}