#include "FreeRTOS.h" // Biblioteca FreeRTOS
#include "hardware/i2c.h" // Biblioteca I2C do Pico
#include "pico/stdlib.h" // Biblioteca padrão do Pico
#include "task.h" // Biblioteca de tarefas do FreeRTOS
#include <stdio.h> // Biblioteca padrão
#include <string.h> // Biblioteca para manipulação de strings

#include "button_handler.h" // Biblioteca de manipulação de botões
#include "display_oled.h" // Biblioteca de exibição OLED
#include "microphone_handler.h" // Biblioteca de manipulação do microfone
#include "rtos_resources.h" // Biblioteca de recursos do RTOS

// Função Principal
int main() {
  // Inicializa a interface padrão de entrada/saída
  stdio_init_all();

  // Configuração dos periféricos
  setup_oled();
  setup_adc();
  setup_dma();
  init_buttons();

  // Criação da tarefa de exibição ociosa
  xTaskCreate(idle_display_task, "IdleDisplay", 256, NULL, 1, NULL);

  // Inicia o escalonador do FreeRTOS
  vTaskStartScheduler();

  while (1) {
  };
}