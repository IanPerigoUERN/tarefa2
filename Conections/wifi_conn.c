#include "wifi_conn.h"
#include "credentials.h"

/**
 * Função: connect_to_wifi
 * Objetivo: Inicializar o chip Wi-Fi da Pico W e conectar a uma rede usando SSID e senha fornecidos.
 */
// wifi_conn.c


  bool wifiConnected = false;

bool connect_to_wifi(const char* ssid, const char* password) {
    printf("[WiFi] Inicializando modo STA...\n");
     // Não inicializa aqui - será feito na task
    int result = cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 15000);
    return (result == 0);
}




///////////TASK do WIFI


void vWifiTask(void *pvParameters) {

    while (true) {
        if (!wifiConnected) {
            printf("Conectando ao WiFi...\n");
            if (connect_to_wifi(WIFI_SSID, WIFI_PASSWORD)) {
                printf("Conexão WiFi bem-sucedida!\n");
                wifiConnected = true;                
                xEventGroupSetBits(xWifi_event,wifi_bits);
            } else {
                printf("Falha na conexão. Tentando novamente...\n");
                vTaskDelay(5000);
            }
        }else{
            if(wifiConnected){
                vTaskDelay(3000);
                
            }else{
                wifiConnected= false;
                mqqtConnected = false;
                xEventGroupClearBits(xWifi_event,wifi_bits);
                xEventGroupClearBits(xMqtt_event, mqtt_bits);                
         }
         vTaskDelay(pdMS_TO_TICKS(10000)); // Verifica a cada 10s
    }
}
}