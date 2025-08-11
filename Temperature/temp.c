#include "temp.h"


//static const float CONVERSION_FACTOR = 3.3f / (1 << 12);

void vTemperatureUpdate(void *pvParameters) {   
    uint16_t adc_value;
    float volt;
     float temperature_float;
    int temperature;
    // Aguarda a conexão USB antes de imprimir
    
    while (true) {
        xEventGroupWaitBits(xMqtt_event, mqtt_bits,pdFALSE,pdTRUE,portMAX_DELAY);
        if(mqqtConnected){
        
        adc_select_input(sensor_temp); // Canal 4 é o sensor de temperatura interno
        adc_value = adc_read();
         volt = (3.3f / 4095) * adc_value;
         temperature_float = 27.0f - (volt - 0.706f) / 0.001721f;
         temperature = (int)(temperature_float + 0.5f);
        printf("[Temp Task] Temperatura lida: %d. Enviando para a fila...\n", temperature);

        // Envia a variável 'temperature' para a fila 'tempQueue'.
        // O 3º parâmetro é o tempo de bloqueio caso a fila esteja cheia.
        // portMAX_DELAY faz com que a task espere indefinidamente até haver espaço.
        if (xQueueSend(tempQueue, &temperature, portMAX_DELAY) == pdPASS) {
            printf("[Temp Task] Dado enviado com sucesso.\n");
        } else {
            printf("[Temp Task] FALHA ao enviar para a fila.\n");
        }
        vTaskDelay(pdMS_TO_TICKS(10000)); // Lê a cada 5 segundos
    }else{
        xEventGroupClearBits(xMqtt_event , mqtt_bits );
    }
    }
        
}

