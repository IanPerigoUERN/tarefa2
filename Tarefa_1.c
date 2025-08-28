

#include "library.h" // Biblioteca principal que inclui todas as dependências necessárias
#include "Global/global.h" // Variável global para EventGroup e bits do wifi e mqtt

#define QUEUE_SIZE 10


QueueHandle_t mpuqueue;
EventGroupHandle_t xWifi_event, xMqtt_event;
TaskHandle_t wifi_h,mqqt_h,mpu_h;

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

    
    gpio_init(led_pin_green);
    gpio_set_dir(led_pin_green, GPIO_OUT);
    
    mpu6050_setup_i2c();       // Configura barramento I2C
    mpu6050_reset();           // Reinicia o sensor

    sleep_ms(1000);
    
    mpuqueue = xQueueCreate(QUEUE_SIZE, sizeof(MPUData_t));
   
    
    
     if (mpuqueue == NULL) {
        printf("Erro ao criar as filas.\n");
        while (true);
    }
    
    
    xTaskCreate(vWifiTask, "Wifi Task", 2048, NULL, 3, &wifi_h);
    xTaskCreate(vMqttTask, "MQTT Task", 1024, NULL, 2, &mqqt_h);
    xTaskCreate(vMPUTask, "MPU Task", 512, NULL, 1, &mpu_h);

    //Core affinity teve que ser iniciado em task.h
    
    vTaskCoreAffinitySet(wifi_h, (1 << 0));
    vTaskCoreAffinitySet(mqqt_h, (1 << 0));  
    vTaskCoreAffinitySet(mpu_h, (1 << 1));
    

    vTaskStartScheduler();

    while(1){

    }
}