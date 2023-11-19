// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html
// https://www.youtube.com/watch?v=FT3h_se0OBA&list=PLgrKXQgo8LPt86ZDjnhrkrVnziC9kkIDm SIMS IOT Devices

// HTTP Client - FreeRTOS ESP IDF - GET

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "wifi_server.h"
#include "wifi_server_task.h"
#include "esp_log.h"
#include "mutex_handler.h"

int permission_status = 0;  //variable that is covered with mutex to poll http server
char http_data[STRMAX]; //http data array

struct http_data_struct sun_location;

void wifi_disconnect() 
{
    // Disconnect wifi clear Ip address, stop DHCP client, remove TCP/UDP related connections

    esp_wifi_disconnect(); //disconnect the wi-fi connectivity
    esp_wifi_stop(); //stop wifi driver
    esp_wifi_deinit(); //unload the wifi driver

}

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(WIFI_TAG, "WiFi is connecting...");
        ESP_LOGI(WIFI_TAG, " ");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(WIFI_TAG, "WiFi connected...");
        ESP_LOGI(WIFI_TAG, " ");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGE(WIFI_TAG, "WiFi lost connection ...");
        ESP_LOGE(WIFI_TAG, " ");
        wifi_disconnect(); //safely disconnect wifi
        esp_restart(); //restart board
        break;
    case IP_EVENT_STA_GOT_IP:
        ESP_LOGI(WIFI_TAG, "WiFi got IP ...");
        ESP_LOGI(WIFI_TAG, " ");
        break;
    default:
        break;
    }
}


void wifi_connection()
{
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
}

void store_data(char* data, int array_len) {
    // splits the http json api into usable data
    bool triggered = false;
    int start_data = 0;
    int distance = 0;
    int counter = 0;
    for (int i = 0; i < array_len; i++) {
        if (data[i] ==',' && triggered) {
            triggered = false;
            distance = i - start_data;
            printf("%d\n", counter);
            printf("%.*s\n", distance, data + start_data);
            if (counter == 2) {
                strncpy(sun_location.date, data + start_data, 12);
                printf("%s\n", sun_location.date);
            }
            else if (counter == 3) {
                strncpy(sun_location.current_time, data + start_data, 14);
                printf("%s\n", sun_location.current_time);
            }
            else if (counter == 4) {
                strncpy(sun_location.sunrise, data + start_data, 7);
                printf("%s\n", sun_location.sunrise);
            }
            else if (counter == 5) {
                strncpy(sun_location.sunset, data + start_data, 7);
                printf("%s\n", sun_location.sunset);
            }
            else if (counter == 6) {
                strncpy(sun_location.sun_status, data + start_data, 3);
                printf("%s\n", sun_location.sun_status);
            }
            else if (counter == 7) {
                strncpy(sun_location.solar_noon, data + start_data, 7);
                printf("%s\n", sun_location.solar_noon);
            }
            else if (counter == 8) {
                strncpy(sun_location.day_length, data + start_data, 7);
                printf("%s\n", sun_location.day_length);
            }
            else if (counter == 9) {
                sun_location.sun_altitude = strtod(data + start_data, data + start_data + 5);
                printf("%f\n", sun_location.sun_altitude);
            }
            else if (counter == 10) {
                sun_location.sun_distance = strtod(data + start_data, data + start_data + 5);
                printf("%f\n", sun_location.sun_distance);
            }
            else if (counter == 11) {
                sun_location.sun_azimuth = strtod(data + start_data, data + start_data + 5);
                printf("%f\n", sun_location.sun_azimuth);
            }
            else if (counter == 12) {
                strncpy(sun_location.moonrise, data + start_data, 7);
                printf("%s\n", sun_location.moonrise);
            }
            else if (counter == 13) {
                strncpy(sun_location.moonset, data + start_data, 7);
                printf("%s\n", sun_location.moonset);
            }
            else if (counter == 14) {
                strncpy(sun_location.moon_status, data + start_data, 3);
                printf("%s\n", sun_location.moon_status);
            }
            else if (counter == 15) {
                sun_location.moon_alitude = strtod(data + start_data, data + start_data + 5);
                printf("%f\n", sun_location.moon_alitude);
            }
            else if (counter == 16) {
                sun_location.moon_distance = strtod(data + start_data, data + start_data + 5);
                printf("%f\n", sun_location.moon_distance);
            }
            else if (counter == 17) {
                sun_location.moon_azimuth = strtod(data + start_data, data + start_data + 5);
                printf("%f\n", sun_location.moon_azimuth);
            }
            counter++;
        } else if (data[i] ==':' && !triggered) {
            triggered = true;
            start_data = i + 1;
        }

    }
}
esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(WIFI_TAG, " %.*s", evt->data_len, (char *)evt->data);
        if(xMutexHttpData != NULL)
        {
            // See if we can obtain the mutex.  If the mutex is unavailable wait 20 ticks to see if it comes free
            if(xSemaphoreTake(xMutexHttpData, (TickType_t) 20) == pdTRUE)
            {

                strcpy(http_data, evt->data); // copy data to http data array
                store_data((char *)evt->data, evt->data_len);
                // Release the Mutex
                xSemaphoreGive(xMutexHttpData);
            }
            else
            {
                ESP_LOGW(WIFI_TAG, "Mutex locked. Unable to access wifi variable safely");
            }}
        break;
    case HTTP_EVENT_ERROR:
        ESP_LOGE(WIFI_TAG, "HTTP EVENT ERROR");
        ESP_LOGE(WIFI_TAG, " ");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(WIFI_TAG, "HTTP EVENT DISCONNECTED");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(WIFI_TAG, "HTTP EVENT ON CONNECTED");
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(WIFI_TAG, "HTTP EVENT ON FINISH");
        break;
        

    default:
        break;
    }
    return ESP_OK;
}

void rest_get()
{   
    /* Ping the server for astrology coordinates on the sun*/
    // create url for server
    char http_url[200] = "https://api.ipgeolocation.io/astronomy?apiKey=";
    strcat(http_url, APIkey);
    strcat(http_url, "&lat=");
    strcat(http_url, latitude);
    strcat(http_url, "&long=");
    strcat(http_url, longitude);
    // printf(http_url);
    // printf("\n");

    //https://api.ipgeolocation.io/astronomy?apiKey=9b5f43cf1bca4aa0a31e5557bab2e815&lat=33.141386&long=-117.302192
    // "http://worldclockapi.com/api/json/utc/now"
    esp_http_client_config_t config_get = {
        .url = http_url,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler,
        .buffer_size = 2048};

    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_set_timeout_ms(client, http_timeout);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

void start_rest_get(void)
{
    /* Start wifi connection to server*/
    wifi_disconnect();
    nvs_flash_init();
    wifi_connection();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    ESP_LOGI(WIFI_TAG, "WIFI was initiated ...........");
    ESP_LOGI(WIFI_TAG, " ");
}