#include "display_oled.h" // Biblioteca para exibição em OLED
#include "rtos_resources.h" // Biblioteca de recursos do RTOS
#include "semphr.h" // Biblioteca de semáforos do FreeRTOS
#include <stdio.h> // Biblioteca padrão 
#include <string.h> // Biblioteca para manipulação de strings
#include "pico/stdlib.h" // Biblioteca padrão pico

// Definição dos pinos I2C
const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

// Buffer do display OLED
uint8_t ssd[ssd1306_buffer_length];

// Função para configuração do display OLED
void setup_oled(){
    // Inicialização da comunicação I2C
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);

    // Definição dos pinos SDA e SCL
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    // Ativação do pull-up
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicialização do display OLED
    ssd1306_init();

    // Limpeza do buffer do display
    memset(ssd, 0, ssd1306_buffer_length);
}

// Função para atualizar a exibição do display OLED
void display_oled(float value, char unit[2]) {
    // Definição da área de renderização 
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    // Cálculo do tamanho do buffer necessário
    calculate_render_area_buffer_length(&frame_area);

    // Limpeza do buffer do display
    memset(ssd, 0, ssd1306_buffer_length);
    
    // Texto a ser exibido no display
    char text[32];
    sprintf(text, "%.2f %s", value, unit);
    
    // Cálculo do comprimento do texto(em pixels) e centralização dele
    int text_length = strlen(text) * 6;
    int x_position = (ssd1306_width - text_length) / 2;
    int y_position = (ssd1306_height - 8) / 2;
    
    // Escrita do texto no buffer 
    ssd1306_draw_string(ssd, x_position, y_position, text);

    // Atualização do display 
    render_on_display(ssd, &frame_area);
}

// Função para exibir a tela de idle no display OLED
void display_oled_idle() {
    // Limpeza do buffer do display
    memset(ssd, 0, ssd1306_buffer_length);
    
    // Definição da área de renderização 
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    ssd1306_draw_string(ssd, 10, 10, "Monitoramento");
    ssd1306_draw_string(ssd, 10, 25, "Press A: Temp");
    ssd1306_draw_string(ssd, 10, 35, "Press B: Mic");

    // Atualização do display 
    render_on_display(ssd, &frame_area);
}

// Tarefa para exibir a tela de idle no display OLED
void idle_display_task(void *params) {
    display_oled_idle();
    vTaskDelete(NULL);
}