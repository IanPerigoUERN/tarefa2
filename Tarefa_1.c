

#include "library.h" // Biblioteca principal que inclui todas as dependências necessárias
#include "Global/global.h" // Variável global para EventGroup e bits do wifi e mqtt

#define QUEUE_SIZE 10


 QueueHandle_t tempQueue, joystickQueue;
EventGroupHandle_t xWifi_event, xMqtt_event;
TaskHandle_t wifi_h,mqqt_h,temp_h,joy_h;

void main()
{
    xWifi_event = xEventGroupCreate();
    xMqtt_event = xEventGroupCreate();
    
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("Falha ao inicializar WiFi\n");
        while(1); // Para aqui se falhar
    }
    cyw43_arch_enable_sta_mode();
    printf("WiFi inicializado com sucesso\n");

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
    
    
    xTaskCreate(vWifiTask, "Wifi Task", 2048, NULL, 3, &wifi_h);
    xTaskCreate(vMqttTask, "MQTT Task", 256, NULL, 2, &mqqt_h);
    xTaskCreate(vShowLastJoystickDirection, "show_joystick_direction", 128, NULL, 1, &joy_h);
    xTaskCreate(vTemperatureUpdate, "Temperature Task", 256, NULL, 1, &temp_h);

    //Core affinity teve que ser iniciado em task.h
    
    vTaskCoreAffinitySet(wifi_h, (1 << 0));
    vTaskCoreAffinitySet(mqqt_h, (1 << 0));  
    vTaskCoreAffinitySet(joy_h, (1 << 1));
    vTaskCoreAffinitySet(temp_h, (1 << 1 ));
   

    vTaskStartScheduler();

    while(1){

    }
}