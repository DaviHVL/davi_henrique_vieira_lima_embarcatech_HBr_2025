// Bibliotecas e Arquivos incluídos para o Projeto
#include <stdio.h>
#include <stdint.h>
#include "adc_to_celsius.h"

// Função para receber o valor digital do sensor interno e converter para Celsius
float adc_to_celsius(uint16_t adc_val){
    float voltage = adc_val * (V_REF / ADC_RESOLUTION); // Converte leitura digital em tensão (volts)
    float temperature_celcius = 27.0f - (voltage - 0.706f) / 0.001721f; // Fórmula de conversão da tensão para temperatura
    return temperature_celcius;
}

