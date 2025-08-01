#ifndef MQTT_COMM_H
#define MQTT_COMM_H

#include "./library.h" // Biblioteca principal que inclui todas as dependências necessárias
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);
bool mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len);
void vMqttTask(void *pvParameters);

#endif