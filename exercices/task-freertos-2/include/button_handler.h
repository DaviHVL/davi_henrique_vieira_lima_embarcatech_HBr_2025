#ifndef BUTTON_HANDLER_H
#include "pico/stdlib.h" // Biblioteca padrão pico 

#define BUTTON_A_PIN 5 // Pino do Botão A
#define BUTTON_B_PIN 6 // Pino do Botão B

// Função para inicializar os botões
extern void init_buttons();

// Callback para lidar com eventos dos botões
static void buttons_callback(uint gpio, uint32_t events);

#endif // !BUTTON_HANDLER_H
