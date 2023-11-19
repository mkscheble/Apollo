#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#define SSID "SharkinPB"
#define PASS "mistylake255"

#define APIkey "9b5f43cf1bca4aa0a31e5557bab2e815"
#define latitude "33.141386"
#define longitude "-117.302192"
#define STRMAX 2048

struct http_data_struct {
    char date[15];
    char current_time[15];
    char sunrise[10];
    char sunset[10];
    char sun_status[10];
    char solar_noon[10];
    char day_length[10];
    double sun_altitude;
    double sun_distance;
    double sun_azimuth;
    char moonrise[10];
    char moonset[10];
    char moon_status[10];
    double moon_alitude;
    double moon_distance;
    double moon_azimuth;
};

extern int permission_status;
extern char http_data[STRMAX];
static const int http_timeout = 30000; //ms

void start_rest_get(void);

void rest_get(void);

#endif
