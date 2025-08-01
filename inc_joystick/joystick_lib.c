#include "joystick_lib.h"

#define SW 22 // Pino de leitura do botão do joystick
ssd1306_t disp;



// Inicializa o joystick configurando o ADC (Conversor Analógico-Digital)
void joystick_init(){
    adc_init();
    adc_gpio_init(26); // Configura o pino GPIO 26 como entrada ADC Y
    adc_gpio_init(27); // Configura o pino GPIO 27 como entrada ADC X

    gpio_init(SW);             // Inicializa o pino do botão
    gpio_set_dir(SW, GPIO_IN); // Configura o pino do botão como entrada
    gpio_pull_up(SW);
}

// Função que retorna a posição do joystick no eixo Y
int value_of_the_pos_of_joystick_y(){
    uint16_t vry_value; // Variável para armazenar o valor lido do ADC

    adc_select_input(0); // Seleciona o canal 0 do ADC (eixo Y do joystick)
    sleep_us(2); // Pequeno delay para garantir estabilidade na leitura
    vry_value = adc_read(); // Lê o valor analógico do eixo Y (0 a 4095)
       
    // Verifica se o joystick foi movido para cima
    if(vry_value < 500){
        return 1;  // Indica movimento para cima
    }
    // Verifica se o joystick foi movido para baixo
    else if(vry_value > 3500){
        return -1; // Indica movimento para baixo
    }
    // Se estiver no meio, considera como neutro
    else{
        return 0;  // Sem movimento significativo
    }
}

// Função que detecta se houve movimento no joystick no eixo Y
int read_movement_of_joystick_y(){
    uint16_t vry_value; // Variável para armazenar a leitura do ADC

    adc_select_input(0); // Seleciona o canal 0 do ADC (eixo Y do joystick)
    sleep_us(2); // Pequeno delay para garantir estabilidade na leitura
    vry_value = adc_read(); // Lê o valor do eixo Y do joystick
        
    // Se o valor estiver fora da faixa central, considera que houve movimento
    if(vry_value < 500 || vry_value > 3500){
        return 1; // Movimento detectado
    }else{
        return 0; // Nenhum movimento detectado
    }
}

// Função que retorna a posição do joystick no eixo X
int value_of_the_pos_of_joystick_x(){
    uint16_t vrx_value; // Variável para armazenar o valor lido do ADC

    adc_select_input(1); // Seleciona o canal 1 do ADC (eixo X do joystick)
    sleep_us(2); // Pequeno delay para garantir estabilidade na leitura
    vrx_value = adc_read(); // Lê o valor analógico do eixo X (0 a 4095)
       
    // Verifica se o joystick foi movido para a esquerda
    if(vrx_value < 500){
        return 1;  // Indica movimento para a esquerda
    }
    // Verifica se o joystick foi movido para a direita
    else if(vrx_value > 3500){
        return -1; // Indica movimento para a direita
    }
    // Se estiver no meio, considera como neutro
    else{
        return 0;  // Sem movimento significativo
    }
}

// Função que detecta se houve movimento no joystick no eixo X
int read_movement_of_joystick_x(){
    uint16_t vrx_value; // Variável para armazenar a leitura do ADC

    adc_select_input(1); // Seleciona o canal 1 do ADC (eixo X do joystick)
    sleep_us(2); // Pequeno delay para garantir estabilidade na leitura
    vrx_value = adc_read(); // Lê o valor do eixo X do joystick
        
    // Se o valor estiver fora da faixa central, considera que houve movimento
    if(vrx_value < 500 || vrx_value > 3500){
        return 1; // Movimento detectado
    }else{
        return 0; // Nenhum movimento detectado
    }
}

float read_percentage_of_move_x(){
    uint16_t vrx_value; // Variável para armazenar a leitura do ADC

    adc_select_input(1); // Seleciona o canal 1 do ADC (eixo X do joystick)
    sleep_us(2); // Pequeno delay para garantir estabilidade na leitura
    vrx_value = adc_read(); // Lê o valor do eixo X do joystick

    // Calcula a porcentagem de movimento no eixo X
    return (vrx_value-2047)/2047.0 * 100;
}

float read_percentage_of_move_y(){
    uint16_t vry_value; // Variável para armazenar a leitura do ADC

    adc_select_input(0); // Seleciona o canal 0 do ADC (eixo Y do joystick)
    sleep_us(2); // Pequeno delay para garantir estabilidade na leitura
    vry_value = adc_read(); // Lê o valor do eixo Y do joystick

    // Calcula a porcentagem de movimento no eixo Y
    return (vry_value-2047)/2047.0 * 100;
}

int read_value_x(){
    uint16_t vrx_value; // Variável para armazenar a leitura do ADC

    adc_select_input(1); // Seleciona o canal 1 do ADC (eixo X do joystick)
    sleep_us(2); // Pequeno delay para garantir estabilidade na leitura
    vrx_value = adc_read(); // Lê o valor do eixo X do joystick

    return vrx_value; // Retorna o valor lido do eixo X
}

int read_value_y(){
    uint16_t vry_value; // Variável para armazenar a leitura do ADC

    adc_select_input(0); // Seleciona o canal 0 do ADC (eixo Y do joystick)
    sleep_us(2); // Pequeno delay para garantir estabilidade na leitura
    vry_value = adc_read(); // Lê o valor do eixo Y do joystick

    return vry_value; // Retorna o valor lido do eixo Y
}




void vShowLastJoystickDirection(void *pvParameters)
{
    JoystickDirection_t currentDirection;
    JoystickDirection_t lastDirection = CENTER;
    // Inicializa o display OLED

    ssd1306_init_display(&disp);
        show_direction_of_joystick(&disp, "Centro");
        currentDirection = CENTER;

        for (;;)
        {
            // Read the joystick position on the Y axis
            int joystick_y = value_of_the_pos_of_joystick_y();

            // Read the joystick position on the X axis
            int joystick_x = value_of_the_pos_of_joystick_x();

            // Blink the LED based on the joystick position
            if (joystick_y == 1) {
                show_direction_of_joystick(&disp, "Baixo");
                currentDirection = DOWN;

            } else if (joystick_y == -1) {
                show_direction_of_joystick(&disp, "Cima");
                currentDirection = UP;
                
            }

            if(joystick_x == 1) {
                show_direction_of_joystick(&disp, "Esquerda");
                currentDirection = LEFT;

            } else if (joystick_x == -1) {
                show_direction_of_joystick(&disp, "Direita");
                currentDirection = RIGHT;
            }

        if (currentDirection != lastDirection) {
            xQueueSend(joystickQueue, &currentDirection, 0);
            lastDirection = currentDirection;
        }

        vTaskDelay(pdMS_TO_TICKS(100)); 
        
    }
}