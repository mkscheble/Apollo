#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mutex_handler.h"
#include "wifi_server.h"
#include "wifi_server_task.h"
#include "esp_log.h"

SemaphoreHandle_t xMutexPermission = NULL;
SemaphoreHandle_t xMutexHttpData = NULL;

// A task that creates the Mutex.
void vCreateMutex()
{
   // Create the Mutex to guard a shared resource.
   xMutexPermission = xSemaphoreCreateMutex();
   xMutexHttpData = xSemaphoreCreateMutex();
}

// Change global parameter that initializes coordinate server call. 
// 6/30/2023 MS was initially going to be used to change coordinate parameter from within the ISR. Ran into issue where mcu would reboot do to calls from within ISR
void vhttpPermission(example_queue_element_t param)
{
   if(xMutexPermission != NULL)
   {
       // See if we can obtain the mutex.  If the mutex is unavailable wait 20 ticks to see if it comes free
       if(xSemaphoreTake(xMutexPermission, (TickType_t) 20) == pdTRUE)
       {
           // We were able to access mutex.
           permission_status = param.permission_granted;
           // Release the Mutex
           xSemaphoreGive(xMutexPermission);
       }
       else
       {
            ESP_LOGW(WIFI_TAG, "Mutex locked. Unable to access variable safely.");
       }
   }
}