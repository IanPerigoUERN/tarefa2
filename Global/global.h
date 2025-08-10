#ifndef GLOBAL_H
#define GLOBAL_H

#include "./library.h"


typedef enum {
    CENTER ,
    UP,
    DOWN,
    LEFT,
    RIGHT
} JoystickDirection_t;

extern QueueHandle_t tempQueue;
extern QueueHandle_t joystickQueue;


// Definindo variáveis do evento wifi e mqtt como tipo EventGroupHandle_t
extern EventGroupHandle_t xWifi_event, xMqtt_event;

#define wifi_bits (1<<0)
#define mqtt_bits (1<<0)












#endif // GLOBAL_H