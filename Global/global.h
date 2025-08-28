#ifndef GLOBAL_H
#define GLOBAL_H

#include "./library.h"


extern EventGroupHandle_t xWifi_event, xMqtt_event;

extern QueueHandle_t mpuqueue;

#define wifi_bits (1<<0)
#define mqtt_bits (1<<0)


#endif // GLOBAL_H