#ifndef HAL_LED_H
#define HAL_LED_H

// Declaração da função responsável por inicializa o LED, apresentado uma camada de abstração ao usuário
void hal_led_init();

// Declaração da função responsável por mudar o estado do LED, apresentado uma camada de abstração ao usuário
void hal_led_toggle();

#endif