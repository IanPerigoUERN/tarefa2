#include "wifi_conn.h"
#include "credentials.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"
/**
 * Função: connect_to_wifi
 * Objetivo: Inicializar o chip Wi-Fi da Pico W e conectar a uma rede usando SSID e senha fornecidos.
 */
// wifi_conn.c


  bool wifiConnected = false;

  char ip_str[16];

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
                
                // --- Seção para obter o IP sem a função cyw43_tcpip_get_netif() ---
                sleep_ms(2000); // Espera o DHCP

                // Acesse a interface de rede diretamente do array de interfaces do lwIP.
                // A interface Wi-Fi (STA) geralmente é a de índice 0 no array netif.
                struct netif *netif = &cyw43_state.netif[CYW43_ITF_STA];
                
                if (netif) {
                    if (ip4_addr_isany_val(netif->ip_addr)) {
                        printf("Ainda não foi atribuído um endereço IP pelo DHCP.\n");
                    } else {
                        
                        ip4addr_ntoa_r(&netif->ip_addr, ip_str, sizeof(ip_str));
                        sleep_ms(1000);
                        printf("Endereço IP obtido: %s\n", ip_str);
                        sleep_ms(5000);
                    }
                } else {
                    printf("Falha ao obter a interface de rede.\n");
                }
                // --------------------------------------------------------------------

                xEventGroupSetBits(xWifi_event, wifi_bits);
            } else {
                printf("Falha na conexão. Tentando novamente...\n");
                vTaskDelay(pdMS_TO_TICKS(5000));
            }
        } else {
            // Lógica de verificação da conexão...
            if (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) {
                // ...
            }
            vTaskDelay(pdMS_TO_TICKS(10000));
        }
    }
}
