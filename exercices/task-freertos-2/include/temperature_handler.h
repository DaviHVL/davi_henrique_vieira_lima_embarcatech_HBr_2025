#ifndef TEMPERATURE_HANDLER_H
#define TEMPERATURE_HANDLER_H

#include <stdint.h> // Biblioteca padrão de inteiros

#define ADC_TEMPERATURE_CHANNEL 4 // Canal ADC para o sensor de temperatura

// Valor da temperatura em graus Celsius
extern double temperature;

// Tarefa para obter a temperatura em graus Celsius
extern void get_celsius(void *pvParameters);

#endif // !TEMPERATURE_HANDLER_H
