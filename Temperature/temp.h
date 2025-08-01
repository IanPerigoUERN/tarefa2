#ifndef TEMP_H
#define TEMP_H


#include "./library.h"

#define sensor_temp 4



// AHT10 Configure

// AHT10 I2C settings - Conforme documentação oficial
#define AHT10_I2C_ADDR 0x38
#define AHT10_CMD_INIT 0xE1      // Comando de inicialização
#define AHT10_CMD_MEASURE 0xAC   // Comando de medição
#define AHT10_CMD_SOFT_RESET 0xBA // Comando de reset
#define AHT10_STATUS_BUSY 0x80   // Bit de status ocupado
#define AHT10_STATUS_CAL 0x08    // Bit de calibração

#define I2C_PORT_SENSOR i2c0
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1


void vTemperatureUpdate(void *pvParameters);
void vTempHumi (void *pvParameters);
#endif