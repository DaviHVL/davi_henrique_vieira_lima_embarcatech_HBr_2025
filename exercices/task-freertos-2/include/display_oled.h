#ifndef DISPLAY_OLED_H
#define DISPLAY_OLED_H
#include "hardware/i2c.h" // Biblioteca I2C do Pico
#include "inc/ssd1306.h" // Biblioteca SSD1306 para OLED
#include "inc/ssd1306_i2c.h" // Biblioteca I2C do SSD1306

extern const uint I2C_SDA; // Pino SDA do I2C
extern const uint I2C_SCL; // Pino SCL do I2C
extern uint8_t ssd[ssd1306_buffer_length]; // Buffer do display OLED

// Função para inicializar o display OLED	
void setup_oled();

// Função para exibir um valor no display OLED
void display_oled(float value, char unit[2]);

// Função para exibir uma tela de idle no display OLED
void display_oled_idle();

// Tarefa para exibir a tela de idle no display OLED
void idle_display_task(void *params);
#endif // DISPLAY_OLED_H