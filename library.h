#include "MPU6050/mpu6050_i2c.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"


///FreeRTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "event_groups.h"

#include <stdio.h>
#include <math.h>
#include "hardware/adc.h"

///LOCAL

#include "Blink/new_blink.h"



#include "Global/global.h" // Variável global para a fila de temperatura


// Conections
#include "pico/cyw43_arch.h"  
#include "lwip/apps/mqtt.h"       // Biblioteca MQTT do lwIP
#include "Conections/mqtt_conn.h"    // Header file com as declarações locais
#include "Conections/wifi_conn.h"    // Header file com as declarações locais
// #include "lwip/netif.h" // Necessário para a struct netif
// #include "lwip/sockets.h"
// Base: https://github.com/BitDogLab/BitDogLab-C/blob/main/wifi_button_and_led/lwipopts.h
#include "lwipopts.h"             // Configurações customizadas do lwIP
#include <stdbool.h>
#include "Conections/credentials.h" // Credenciais do WiFi e MQTT
#include "lwip/dns.h"