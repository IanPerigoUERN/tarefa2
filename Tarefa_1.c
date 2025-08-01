// #include "pico/stdlib.h"
// #include "FreeRTOS.h"
// #include "task.h"
// #include <stdio.h>
// #include "inc_joystick/joystick_lib.h"
// #include "inc_OLED/ssd1306.h"
// #include "Temperature/temp.h"
// #include "Blink/new_blink.h"

#include "library.h" // Biblioteca principal que inclui todas as dependências necessárias
#include "Global/global.h" // Variável global para a fila de temperatura


#define QUEUE_SIZE 10


QueueHandle_t tempQueue;
QueueHandle_t joystickQueue;

void main()
{
    
    stdio_init_all();
    joystick_init();
    gpio_init(led_pin_green);
    gpio_set_dir(led_pin_green, GPIO_OUT);
    adc_set_temp_sensor_enabled(true);
    
    sleep_ms(10000);
    tempQueue = xQueueCreate(QUEUE_SIZE, sizeof(int));
    joystickQueue = xQueueCreate(QUEUE_SIZE, sizeof(JoystickDirection_t));

     if (tempQueue == NULL || joystickQueue == NULL) {
        printf("Erro ao criar as filas.\n");
        while (true);
    }

    printf("Criado as filas.\n");
    
    xTaskCreate(vWifiTask, "Wifi Task", 256, NULL, 3, NULL);
    xTaskCreate(vMqttTask, "MQTT Task", 256, NULL, 2, NULL);
    xTaskCreate(vShowLastJoystickDirection, "show_joystick_direction", 128, NULL, 1, NULL);
    xTaskCreate(vTemperatureUpdate, "Temperature Task", 256, NULL, 1, NULL);
   

    vTaskStartScheduler();

    while(1){

    }
}