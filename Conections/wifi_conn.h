#ifndef WIFI_CONN_H
#define WIFI_CONN_H
#include "./library.h"
bool connect_to_wifi(const char* ssid, const char* password);
void vWifiTask(void *pvParameters);


#endif