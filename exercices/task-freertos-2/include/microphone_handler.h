#ifndef MICROPHONE_HANDLER_H
#define MICROPHONE_HANDLER_H

#include "FreeRTOS.h" // Biblioteca FreeRTOS
#include "task.h" // Biblioteca de tarefas do FreeRTOS
#include "hardware/adc.h" // Biblioteca ADC do Pico
#include "hardware/dma.h" // Biblioteca DMA do Pico

#define MIC_CHANNEL 2 // Canal ADC do microfone
#define MIC_ADC_PIN 28 // Pino do ADC
#define ADC_CLOCK_DIV 96.f // Divisor de clock do ADC
#define SAMPLES 200 // Número de amostras 
#define ADC_RESOLUTION (1 << 12)  // Resolução do ADC (12-bit ADC -> 4096 níveis)
#define V_REF 3.3f  // Tensão de referência do ADC
#define MIC_OFFSET 1.65f  // Offset do sinal do microfone (meio da escala de 3.3V)

extern uint dma_channel; // Canal DMA
extern dma_channel_config dma_cfg; // Configuração do DMA
extern uint16_t adc_buffer[SAMPLES]; // Buffer de armazenamento de amostras de ADC

// Funções para configurar o ADC 
void setup_adc();

// Funções para configurar o DMA
void setup_dma();

// Função para amostrar o microfone
void sample_mic();

// Função para calcular a potência RMS do sinal do microfone
float mic_rms();

// Função para converter a potência RMS em decibéis (dB)
float convert_to_dB(float v_rms);

// Variável de Handle para a tarefa do microfone
extern TaskHandle_t microphone_task_handle;

// Tarefa para processar o microfone
void microphone_task(void *pvParameters);
#endif // MICROPHONE_HANDLER_H