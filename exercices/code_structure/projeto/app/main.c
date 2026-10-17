#include "pico/stdlib.h"
#include "../include/hal_led.h"

int main() {
    // Inicializa todas as funcionalidades padrão de entrada/saída
    stdio_init_all();

    // Inicializa o LED através da camada de abstração (HAL)
    hal_led_init();

    // Loop infinito da aplicação principal
    while (1) {
        // Alterna o estado do LED
        hal_led_toggle();
        // Aguarda 500 milissegundos
        sleep_ms(500);
    }

    return 0;
}
