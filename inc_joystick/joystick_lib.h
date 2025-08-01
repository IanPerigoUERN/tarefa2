#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "./library.h"

// Inicializa o joystick configurando o ADC (Conversor Analógico-Digital)
void joystick_init();

// Função que retorna a posição do joystick no eixo Y
int value_of_the_pos_of_joystick_y();

// Função que detecta se houve movimento no joystick no eixo Y
int read_movement_of_joystick_y();

// Função que retorna a porcentagem da posição do joystick no eixo Y
float read_percentage_of_move_y();

// Função que retorna a posição do joystick no eixo X
int value_of_the_pos_of_joystick_x();

// Função que detecta se houve movimento no joystick no eixo X
int read_movement_of_joystick_x();

// Função que retorna a porcentagem da posição do joystick no eixo X
float read_percentage_of_move_x();

// Função que lê o valor do eixo X do joystick
int read_value_x();

// Função que lê o valor do eixo Y do joystick
int read_value_y();

void vShowLastJoystickDirection(void *pvParameters);

#endif