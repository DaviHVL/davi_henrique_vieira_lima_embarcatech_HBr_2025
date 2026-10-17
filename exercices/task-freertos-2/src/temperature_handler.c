#include "temperature_handler.h" // Biblioteca de manipulação de temperatura
#include "display_oled.h" // Biblioteca de manipulação do OLED
#include "hardware/adc.h" // Biblioteca ADC do Pico
#include "rtos_resources.h" // Biblioteca de recursos do RTOS

// Variável global para armazenar a temperatura em graus Celsius
double temperature = 0;

// Função para ler o valor do ADC e converter para temperatura em Fahrenheit
double adc_to_fahrenheit(uint16_t adc_value) {
  const float conversion_factor = 3.3f / (1 << 12);
  double voltage = adc_value * conversion_factor;
  double temp = 27.0f - (voltage - 0.706f) / 0.001721f;
  return (temp * 1.8) + 32;
}

// Função para converter Fahrenheit para Celsius
double fahrenheit_to_celsius(float fahrenheit) {
  return (fahrenheit - 32) * (5.0 / 9.0);
}

// Tarefa para ler o valor do ADC e exibir a temperatura em graus Celsius no OLED
void get_celsius(void *pvParameters) {
  while (1) {
    uint16_t adc_value = adc_read();
    double fahrenheit = adc_to_fahrenheit(adc_value);
    temperature = fahrenheit_to_celsius(fahrenheit);
    display_oled(temperature, "C");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
