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


#endif // GLOBAL_H