#include "mpu6050_i2c.h"


#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1
#define MPU6050_ADDR 0x68
//0x6b
//3b (todos os dados)

void mpu6050_setup_i2c() {
    i2c_init(I2C_PORT, 400*1000); // common options: 100*1000 (100 kHz) or 400*1000 (400 kHz)
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

void mpu6050_reset() {
    uint8_t buf[] = {0x68, 0x80};
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, buf, 2, false);
    sleep_ms(100);
    buf[1] = 0x00;
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, buf, 2, false);
    sleep_ms(10);
}

uint8_t mpu6050_get_accel_range() {
    uint8_t reg = 0x1C;
    uint8_t val;
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, &val, 1, false);
    return (val >> 3) & 0x03; // bits 4:3
}

// 0=±2g, 1=±4g, 2=±8g, 3=±16g
void mpu6050_set_accel_range(uint8_t range) {
    uint8_t buf[2];
    buf[0] = 0x1C; // ACCEL_CONFIG register
    buf[1] = range << 3; // bits 3 e 4
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, buf, 2, false);
}

void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    uint8_t buffer[6];
    uint8_t reg = 0x3B; //MPU6050_REG_ACCEL_XOUT_H
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, buffer, 6, false);
    for (int i=0; i<3; i++)
        accel[i] = (buffer[2*i]<<8) | buffer[2*i+1];

    reg = 0x43; //MPU6050_REG_GYRO_XOUT_H
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, buffer, 6, false);
    for (int i=0; i<3; i++)
        gyro[i] = (buffer[2*i]<<8) | buffer[2*i+1];

    reg = 0x41; //MPU6050_REG_TEMP_OUT_H
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, buffer, 2, false);
    *temp = (buffer[0]<<8) | buffer[1];
}




MPUData_t mpuData;

void vMPUTask (void *pvParameters) {
    
    mpuData.temp= 0;
    mpuData.temp_celsius = 0.0;


    while(true) {
        xEventGroupWaitBits(xMqtt_event, mqtt_bits, pdFALSE, pdTRUE, portMAX_DELAY);
        mpu6050_read_raw(mpuData.accel, mpuData.gyro, &mpuData.temp); // Lê valores
        mpuData.temp_celsius = 0.0;
        vTaskDelay(1000);

        printf("Accel X: %d, Y: %d, Z: %d | Gyro X: %d, Y: %d, Z: %d | Temp: %.2f\n",
               mpuData.accel[0]/ 16384, 
               mpuData.accel[1]/ 16384, 
               mpuData.accel[2]/ 16384,

               mpuData.gyro[0]/ 131, 
               mpuData.gyro[1]/ 131, 
               mpuData.gyro[2]/ 131,

               mpuData.temp_celsius = (mpuData.temp / 340.0) + 36.53);


         // Envia para fila (sem bloquear caso cheia)
            if (xQueueSend(mpuqueue, &mpuData, 0) != pdPASS) {
            printf("Fila cheia, descartando leitura.\n");
        }


     

        vTaskDelay(pdMS_TO_TICKS(100)); // 10Hz
    }
}
