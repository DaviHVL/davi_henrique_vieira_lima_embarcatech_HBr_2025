// Bibliotecas incluídas para o Projeto
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "inc/ssd1306.h"
#include "inc/ssd1306_i2c.h"

// Definição dos Pinos e Constantes do Sistema
#define VRX 27 // Pino GPIO referente ao eixo X do joystick
#define VRY 26 // Pino GPIO referente ao eixo Y do joystick
#define ADC_CHANNEL_0 0 // Canal ADC referente ao pino VRX
#define ADC_CHANNEL_1 1 // Canal ADC referente ao pino VRY

// Definição de variáveis globais
uint16_t valor_x; // Valor digital referente ao eixo X
uint16_t valor_y; // Valor digital referente ao eixo Y

const uint I2C_SDA = 14; // Pino SDA do I2C
const uint I2C_SCL = 15; // Pino SCL do I2C
uint8_t ssd[ssd1306_buffer_length]; // Buffer do display OLED

// Funções

// Função de configuração do módulo ADC
void setup_adc(){
    adc_init(); // Inicializa o sistema de ADC
    
    adc_gpio_init(VRX); // Habilita o uso analógico no GPIO27
    adc_gpio_init(VRY); // Habilita o uso analógico no GPIO26
}

// Função para ler os valores digitais dos canais ADC e conversão para valores digitais
void read_joystick(){
    // Seleciona canal 0  e lê o valor
    adc_select_input(ADC_CHANNEL_0);
    sleep_ms(1);
    valor_y = adc_read();

    // Seleciona canal 1  e lê o valor
    adc_select_input(ADC_CHANNEL_1);
    sleep_ms(1);
    valor_x = adc_read();
}

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
void display_oled() {
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
    
    // Criação das strings de texto
    char linha1[32];
    char linha2[32];
    
    sprintf(linha1, "Eixo X: %u", valor_x);
    sprintf(linha2, "Eixo Y: %u", valor_y);

    // Cálculo das posições centralizadas
    int x1 = (ssd1306_width - strlen(linha1) * 6) / 2;
    int x2 = (ssd1306_width - strlen(linha2) * 6) / 2;

    // y = altura em pixels. Como cada linha tem 8px de altura:
    int y1 = (ssd1306_height / 2) - 15; // Primeira linha
    int y2 = (ssd1306_height / 2);     // Segunda linha

    // Desenho das duas linhas
    ssd1306_draw_string(ssd, x1, y1, linha1);
    ssd1306_draw_string(ssd, x2, y2, linha2);

    // Atualização do display 
    render_on_display(ssd, &frame_area);
}

// Função Principal
int main() {
    // Inicializa a interface padrão de entrada/saída
    stdio_init_all();

    // Configuração dos periféricos
    setup_adc();
    setup_oled();

    // Loop principal do programa
    while(true){
        // Leitura dos valores analógicos e conversão para valores digitais
        read_joystick();
        // Atualização do display OLED
        display_oled();

        sleep_ms(100);
    }  
}
