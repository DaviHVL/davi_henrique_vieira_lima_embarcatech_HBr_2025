#include "../include/hal_led.h"
#include "../include/led_embutido.h"

// Declaração da variável estática que representa o estado do LED
static bool estado = false;

// Implementação da função responsável por inicializa o LED, apresentado uma camada de abstração ao usuário
void hal_led_init() {
    led_embutido_init();
}

// Implementação da função responsável por mudar o estado do LED, apresentado uma camada de abstração ao usuário
void hal_led_toggle() {
    estado = !estado;
    led_embutido_set(estado);
}