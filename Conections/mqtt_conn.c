
#include "mqtt_conn.h"    // Header file com as declarações locais
#include "credentials.h"
#include "queue.h"



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
        0,  // Não reter
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



//// DNS


extern bool wifiConnected; // Variável global para verificar conexão Wi-Fi

extern QueueHandle_t tempQueue; // Fila para receber dados de temperatura



 void vMqttTask(void *pvParameters) {
    JoystickDirection_t joystickDirection;
    int temp = 0;
    char mensagem[64];

    TickType_t lastTempPublishTime = 0;
    const TickType_t tempInterval = pdMS_TO_TICKS(30000); // 30 segundos
    bool primeiraPublicacaoFeita = false;

    for (;;) {
        if (wifiConnected) {
           
            TickType_t now = xTaskGetTickCount();  

            //  Publicação imediata da temperatura (somente uma vez ao conectar)
            if (!primeiraPublicacaoFeita) {
                if (xQueueReceive(tempQueue, &temp, 0)) {
                    snprintf(mensagem, sizeof(mensagem), "%d", temp);
                    printf("[MQTT Task] Publicando temperatura inicial: %s\n", mensagem);
                    mqtt_comm_publish(CANAL_DO_BROKER_TEMP, mensagem, strlen(mensagem));
                    lastTempPublishTime = now;  // zera o cronômetro
                    primeiraPublicacaoFeita = true;
                }
            }

            //  Joystick: publica ao mudar
            if (xQueueReceive(joystickQueue, &joystickDirection, 0)) {
                const char *direcaoStr = NULL;
                switch (joystickDirection) {
                    case UP: direcaoStr = "CIMA"; break;
                    case DOWN: direcaoStr = "BAIXO"; break;
                    case LEFT: direcaoStr = "ESQUERDA"; break;
                    case RIGHT: direcaoStr = "DIREITA"; break;
                    case CENTER: default: direcaoStr = "CENTRO"; break;
                }

                snprintf(mensagem, sizeof(mensagem), "%s", direcaoStr);
                printf("[MQTT Task] Publicando joystick: %s\n", mensagem);
                mqtt_comm_publish(CANAL_DO_BROKER_JOYSTICK, mensagem, strlen(mensagem));
                
                gpio_put(led_pin_green, 1);
                vTaskDelay(50);
                gpio_put(led_pin_green, 0);

                vTaskDelay(pdMS_TO_TICKS(100));
            }

            //  Temperatura: publica a cada 30 segundos
            if ((now - lastTempPublishTime) >= tempInterval) {
                if (xQueueReceive(tempQueue, &temp, 0)) {
                    snprintf(mensagem, sizeof(mensagem), "%d", temp);
                    printf("[MQTT Task] Publicando temperatura periódica: %s\n", mensagem);
                    mqtt_comm_publish(CANAL_DO_BROKER_TEMP, mensagem, strlen(mensagem));
                    
                    gpio_put(led_pin_green, 1);
                    vTaskDelay(50);
                    gpio_put(led_pin_green, 0);

                    lastTempPublishTime = now;
                }
            }
        } else {
            // Se Wi-Fi cair, resetar flag
            primeiraPublicacaoFeita = false;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}