#include "../include/led_embutido.h"
#include "pico/cyw43_arch.h"

// Implementação da função responsável por inicializa o driver do LED embutido usando cyw43_arch
bool led_embutido_init() {
    return cyw43_arch_init() == 0;
}

// Implementação da função reponsável por ligar ou desligar o LED 
void led_embutido_set(bool on) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on ? 1 : 0);
}
