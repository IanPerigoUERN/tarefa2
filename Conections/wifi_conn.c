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
    if (cyw43_arch_init()) {
        printf("[WiFi] Falha ao inicializar o driver WiFi\n");
        return false;
    }

    cyw43_arch_enable_sta_mode();
    printf("[WiFi] Modo STA habilitado\n");

    int result = cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 10000);
    printf("[WiFi] Resultado da conexão: %d\n", result);

    if (result == 0) {
        printf("[WiFi] Conectado com sucesso\n");
        return true;
    } else {
        printf("[WiFi] Falha ao conectar\n");
        return false;
    }
}



///////////TASK do WIFI


void vWifiTask(void *pvParameters) {
    (void) pvParameters;

    while (true) {
        if (!wifiConnected) {
            printf("Conectando ao WiFi...\n");
            if (connect_to_wifi(WIFI_SSID, WIFI_PASSWORD)) {
                printf("Conexão WiFi bem-sucedida!\n");
                wifiConnected = true;
                mqtt_setup(NOME_DO_DISPOSITIVO, IP_DO_BROKER, USER_DO_BROKER, SENHA_DO_BROKER);
            } else {
                printf("Falha na conexão. Tentando novamente...\n");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Verifica a cada 10s
    }
}