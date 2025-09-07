
#include "mqtt_conn.h"    // Header file com as declarações locais
#include "credentials.h"
#include "queue.h"

 bool mqqtConnected = false;

/* Variável global estática para armazenar a instância do cliente MQTT
 * 'static' limita o escopo deste arquivo */
static mqtt_client_t *client;
static ip_addr_t broker_addr;

/* Callback de conexão MQTT - chamado quando o status da conexão muda
 * Parâmetros:
 *   - client: instância do cliente MQTT
 *   - arg: argumento opcional (não usado aqui)
 *   - status: resultado da tentativa de conexão */
static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status == MQTT_CONNECT_ACCEPTED) {
        printf("Conectado ao broker MQTT com sucesso!\n");
    } else {
        printf("Falha ao conectar ao broker, código: %d\n", status);
    }
}

/* Função para configurar e iniciar a conexão MQTT
 * Parâmetros:
 *   - client_id: identificador único para este cliente
 *   - broker_ip: endereço IP do broker como string (ex: "192.168.1.1")
 *   - user: nome de usuário para autenticação (pode ser NULL)
 *   - pass: senha para autenticação (pode ser NULL) */
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass) {
    ip_addr_t broker_addr;  // Estrutura para armazenar o IP do broker
    
    // Converte o IP de string para formato numérico
    if (!ip4addr_aton(broker_ip, &broker_addr)) {
        printf("Erro no IP\n");
        return;
    }

    // Cria uma nova instância do cliente MQTT
    client = mqtt_client_new();
    if (client == NULL) {
        printf("Falha ao criar o cliente MQTT\n");
        return;
    }

    // Configura as informações de conexão do cliente
    struct mqtt_connect_client_info_t ci = {
        .client_id = client_id,  // ID do cliente
        .client_user = user,     // Usuário (opcional)
        .client_pass = pass      // Senha (opcional)
    };

    // Inicia a conexão com o broker
    // Parâmetros:
    //   - client: instância do cliente
    //   - &broker_addr: endereço do broker
    //   - 1883: porta padrão MQTT
    //   - mqtt_connection_cb: callback de status
    //   - NULL: argumento opcional para o callback
    //   - &ci: informações de conexão
    mqtt_client_connect(client, &broker_addr, 1883, mqtt_connection_cb, NULL, &ci);
}

/* Callback de confirmação de publicação
 * Chamado quando o broker confirma recebimento da mensagem (para QoS > 0)
 * Parâmetros:
 *   - arg: argumento opcional
 *   - result: código de resultado da operação */
static void mqtt_pub_request_cb(void *arg, err_t result) {
    if (result == ERR_OK) {
        printf("Publicação MQTT enviada com sucesso!\n");
    } else {
        printf("Erro ao publicar via MQTT: %d\n", result);
    }
}

/* Função para publicar dados em um tópico MQTT
 * Parâmetros:
 *   - topic: nome do tópico (ex: "sensor/temperatura")
 *   - data: payload da mensagem (bytes)
 *   - len: tamanho do payload */
bool mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len) {
    err_t status = mqtt_publish(
        client,
        topic,
        data,
        len,
        0,  // QoS 0
        0,  // reter
        mqtt_pub_request_cb,
        NULL
    );

    if (status != ERR_OK) {
        printf("[MQTT] mqtt_publish falhou: %d\n", status);
        return false;
    }

    return true;
}



///// TASK DO MQTT 

extern QueueHandle_t mpuqueue; // Fila para receber dados de temperatura

extern MPUData_t mpuData;

 void vMqttTask(void *pvParameters) {
   
    
    char mensagem[512];
    bool mqttSetupFeito = false;

    TickType_t lastTempPublishTime = 0;
    const TickType_t tempInterval = pdMS_TO_TICKS(60000); // 30 segundos
    bool primeiraPublicacaoFeita = false;

    char timestamp[32];

    static int lastAccel[3]={0};
    static int lastGyro[3]={0};

    //theresholds para ignorar variações pequenas

    const int accelThereshold = 200;
    const int gyroThereshold = 200;

    bool MPUChange = false;

        

    while (true) {
        // Aguarda WiFi estar conectado
        xEventGroupWaitBits(xWifi_event, wifi_bits, pdFALSE, pdTRUE, portMAX_DELAY);
        // printf("WiFi confirmado, verificando MQTT...\n");
        
        if (wifiConnected && !mqttSetupFeito) {
            printf("Configurando MQTT...\n");
            mqtt_setup(NOME_DO_DISPOSITIVO, IP_DO_BROKER, USER_DO_BROKER, SENHA_DO_BROKER);
            mqttSetupFeito = true;
            mqqtConnected = true;
            xEventGroupSetBits(xMqtt_event, mqtt_bits);
            vTaskDelay(pdMS_TO_TICKS(2000)); // Aguarda conexão estabilizar
        }

        if (wifiConnected && mqqtConnected) {
            
            
            if (xQueueReceive(mpuqueue, &mpuData, 0)) {
                
                
                bool MPUChange = false;

                
                if (abs(mpuData.accel[0] - lastAccel[0]) > accelThereshold ||
                    abs(mpuData.accel[1] - lastAccel[1]) > accelThereshold ||
                    abs(mpuData.accel[2] - lastAccel[2]) > accelThereshold ||
                    abs(mpuData.gyro[0]  - lastGyro[0])  > gyroThereshold  ||
                    abs(mpuData.gyro[1]  - lastGyro[1])  > gyroThereshold  ||
                    abs(mpuData.gyro[2]  - lastGyro[2])  > gyroThereshold  )
                {
                    MPUChange = true;
                }
                
                TickType_t now_ticks = xTaskGetTickCount();
                
                // Publica se for a primeira vez, se houver mudança OU se o tempo de 60 segundos passou
                if (!primeiraPublicacaoFeita || MPUChange || (now_ticks - lastTempPublishTime) >= tempInterval) {
                     
                    time_t now_time;
                    struct tm timeinfo;
                    time(&now_time);
                    localtime_r(&now_time, &timeinfo);
                    
                    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", &timeinfo);

                    snprintf(mensagem, sizeof(mensagem),
               "{"
                        "\"team\":\"%s\","
                        "\"device\":\"%s\","
                        "\"ssid\":\"%s\","
                        "\"IP\":\"%s\","
                        "\"sensor\":\"MPU-6050\","
                        "\"data\":{"
                            "\"accel\":{"
                                "\"x\":%d,"
                                "\"y\":%d,"
                                "\"z\":%d"
                            "},"
                            "\"gyro\":{"
                                "\"x\":%d,"
                                "\"y\":%d,"
                                "\"z\":%d"
                            "},"
                            "\"temperature\":%.2f"
                        "},"
                        "\"timestamp\":\"%s\""
                    "}",
                            USER_DO_BROKER, NOME_DO_DISPOSITIVO, WIFI_SSID,ip_str,
                             mpuData.accel[0], mpuData.accel[1], mpuData.accel[2],
                             mpuData.gyro[0], mpuData.gyro[1], mpuData.gyro[2],
                             mpuData.temp_celsius,timestamp);
                    mqtt_comm_publish(CANAL_DO_BROKER_MPU, (uint8_t*)mensagem, strlen(mensagem));

                    // Atualiza os últimos valores e o tempo de publicação
                    lastAccel[0] = mpuData.accel[0];
                    lastAccel[1] = mpuData.accel[1];
                    lastAccel[2] = mpuData.accel[2];
                    lastGyro[0]  = mpuData.gyro[0];
                    lastGyro[1]  = mpuData.gyro[1];
                    lastGyro[2]  = mpuData.gyro[2];

                    lastTempPublishTime = now_ticks; // Use now_ticks
                    primeiraPublicacaoFeita = true;

                    gpio_put(led_pin_green, 1);
                    vTaskDelay(pdMS_TO_TICKS(50));
                    gpio_put(led_pin_green, 0);
                }
            }
        } else {
            // Se Wi-Fi cair, reseta as flags
            if (!wifiConnected) {
                mqqtConnected = false;
                mqttSetupFeito = false;
                primeiraPublicacaoFeita = false;
                xEventGroupClearBits(xMqtt_event, mqtt_bits);
                printf("WiFi desconectado, resetando MQTT...\n");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Aumentar delay para 1 segundo
    }
}