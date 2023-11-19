#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "heartbeat.h"
#include "esp_log.h"




// Task executes that continually blinks LED. Shows visual status of the microcontroller
void heartbeatTask()
{
    /* Set the GPIO as a push/pull output */
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    ESP_LOGI(HEARTBEAT_TAG, "Heart Beat Task Started");
    while (1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, Output_Low);
        // portTICK_PEIOD_MS = 1000 / 500 by default
        vTaskDelay(250 / portTICK_PERIOD_MS);  //250 for 75% duty cycle

        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, Output_High);
        vTaskDelay(750 / portTICK_PERIOD_MS); //750 for 75% duty cycle
    }
}


/* Starts heartbeat task with freertos*/
int start_heartbeat_task(void) 
{
    TaskHandle_t heartbeat_handle = NULL;
    if (xTaskCreate(heartbeatTask, "Heartbeat Task", 2048, NULL, tskIDLE_PRIORITY + 1, &heartbeat_handle) != pdPASS)
    {   
        ESP_LOGW(HEARTBEAT_TAG, "Heartbeat Task creation failed! reboot needed.");
        esp_restart();
        return 0;
    }
    return 1;
}