#include "button_handler.h" // Biblioteca de manipulação de botões
#include "display_oled.h" // Biblioteca de exibição OLED
#include "hardware/adc.h" // Biblioteca ADC do Pico
#include "hardware/irq.h" // Biblioteca de interrupções do Pico
#include "microphone_handler.h" // Biblioteca de manipulação do microfone
#include "rtos_resources.h" // Biblioteca de recursos do RTOS
#include "temperature_handler.h" // Biblioteca de manipulação de temperatura
#include <pico/stdlib.h> // Biblioteca padrão do Pico
#include <string.h> // Biblioteca de manipulação de strings

// VAriáveis globais para armazenar o tempo do último pressionamento dos botões
absolute_time_t last_press_time_a = 0;
absolute_time_t last_press_time_b = 0;

// Estado da máquina de estados
state_t state = IDLE;

// Handles para as tarefas do microfone e temperatura
TaskHandle_t mic_handle = NULL;
TaskHandle_t temp_handle = NULL;

// Callback para lidar com eventos dos botões
static void buttons_callback(uint gpio, uint32_t events) {
  if (gpio == 5) {
    if (absolute_time_diff_us(last_press_time_a, get_absolute_time()) >
        200000) {
      last_press_time_a = get_absolute_time();
      if (state == TEMP) {
        vTaskDelete(temp_handle);
        state = IDLE;
        display_oled_idle();
        // Placeholder pra indicar que não está executando uma tarefa
        return;
      }

      adc_select_input(ADC_TEMPERATURE_CHANNEL);
      if (state != IDLE)
        vTaskDelete(mic_handle);

      xTaskCreate(get_celsius, "Temp Read", 256, NULL, 1, &temp_handle);
      state = TEMP;
    }
  } else if (gpio == 6) {
    if (absolute_time_diff_us(last_press_time_b, get_absolute_time()) >
        200000) {
      last_press_time_b = get_absolute_time();

      if (state == MIC) {
        vTaskDelete(mic_handle);
        state = IDLE;
        display_oled_idle();
        // Placeholder pra indicar que não está executando uma tarefa
        return;
      }

      adc_select_input(MIC_CHANNEL); // Seleção do canal de entrada do ADC
      if (state != IDLE)
        vTaskDelete(temp_handle);

      xTaskCreate(microphone_task, "Mic Read", 256, NULL, 1, &mic_handle);
      state = MIC;
    }
  }
}

// Função para inicializar os botões
void init_buttons() {
  gpio_init(BUTTON_A_PIN);
  gpio_init(BUTTON_B_PIN);

  gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
  gpio_set_dir(BUTTON_B_PIN, GPIO_IN);

  gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true,
                                     buttons_callback);
  gpio_set_irq_enabled(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true);

  gpio_pull_up(BUTTON_A_PIN);
  gpio_pull_up(BUTTON_B_PIN);
}
