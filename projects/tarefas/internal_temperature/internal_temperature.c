// Bibliotecas incluídas para o Projeto
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "inc/ssd1306.h"
#include "inc/ssd1306_i2c.h"

// Constantes do Sistema
#define TEMPERATURE_CHANNEL 4 // Canal ADC do sensor de temperatura
#define ADC_RESOLUTION (1 << 12) // Resolução do ADC (12-bit ADC -> 4096 níveis)

// Definição de variáveis globais
uint16_t digital_temperature; // Armazena o valor digital convertido
float temperature_celcius; // Temperatura interna em graus Celcius

const uint I2C_SDA = 14; // Pino SDA do I2C
const uint I2C_SCL = 15; // Pino SCL do I2C
uint8_t ssd[ssd1306_buffer_length]; // Buffer do display OLED

// Funções
// Função de configuração do módulo ADC
void setup_adc() {
    adc_init(); // Inicializa o sistema de ADC
    adc_set_temp_sensor_enabled(true); // Ativa o sensor de temperatura interno da Pico
    adc_select_input(TEMPERATURE_CHANNEL); // Seleciona o canal do sensor de temperatura
}

// Função para ler a temperatura digital e converter para Celsius
void read_temperature(){
    digital_temperature = adc_read(); // Lê valor digital do sensor interno

    float voltage = digital_temperature * (3.3 / ADC_RESOLUTION); // Converte leitura digital em tensão (volts)
    temperature_celcius = 27.0f - (voltage - 0.706f) / 0.001721f; // Fórmula de conversão da tensão para temperatura
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
    
    // Texto a ser exibido no display
    char text[32];
    sprintf(text, "Temp: %.2f  ", temperature_celcius);
    
    // Cálculo do comprimento do texto(em pixels) e centralização dele
    int text_length = strlen(text) * 6;
    int x_position = (ssd1306_width - text_length) / 2;
    int y_position = (ssd1306_height - 8) / 2;
    
    // Escrita do texto no buffer 
    ssd1306_draw_string(ssd, x_position, y_position, text);

    // Atualização do display 
    render_on_display(ssd, &frame_area);
}

// Função Principal
int main(){
    // Inicializa a interface padrão de entrada/saída
    stdio_init_all();

    // Configuração dos periféricos
    setup_adc();
    setup_oled();

    // Loop principal do programa
    while(true){
        read_temperature(); // Lê e converte a temperatura
        display_oled(); // Atualização do display OLED
        sleep_ms(1000);
    }
}
