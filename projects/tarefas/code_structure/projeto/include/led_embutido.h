#ifndef LED_EMBUTIDO_H
#define LED_EMBUTIDO_H

#include <stdbool.h>

// Declaração da função responsável por inicializa o driver do LED embutido usando cyw43_arch
bool led_embutido_init();

// Declaração da função reponsável por ligar ou desligar o LED 
void led_embutido_set(bool on);

#endif