// Bibliotecas incluídas para a Tarefa
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/time.h"
#include "inc/ssd1306.h"
#include "inc/ssd1306_i2c.h"

// Definição dos Pinos 
#define BUTTON_A 5 // Pino do Botão A
#define BUTTON_B 6 // Pino do Botão B

// Definição de variáveis globais
volatile uint num_pressed_buttonB = 0; // Contador de vezes que o botão B foi pressionado
volatile bool buttonA_pressed = false; // Flag para identificar se o botão A foi pressionado

struct repeating_timer timer; // Timer que será usado para contagem regressiva
volatile uint time_passed = 9; // Tempo restante 
volatile bool timer_running = false; // Flag para indicar se o timer está ativo

const uint I2C_SDA = 14; // Pino SDA do I2C
const uint I2C_SCL = 15; // Pino SCL do I2C
uint8_t ssd[ssd1306_buffer_length]; // Buffer do display OLED
volatile bool update_display = true; // Flag para indicar se é necessário atualizar o display OLED

// Funções

// Função para configuração dos botões
void setup_buttons() {
    gpio_init(BUTTON_A); // Inicialização do pino 
    gpio_set_dir(BUTTON_A, GPIO_IN); // Definição do pino como entrada
    gpio_pull_up(BUTTON_A); // Ativação do pull-up

    gpio_init(BUTTON_B); // Inicialização do pino 
    gpio_set_dir(BUTTON_B, GPIO_IN); // Definição do pino como entrada
    gpio_pull_up(BUTTON_B); // Ativação do pull-up
}

// Função para configuração da callback do timer
bool repeating_timer_callback(struct repeating_timer *t) {
    // Verifica se já se passaram 9 segundos
    if (time_passed > 0) {
        time_passed--; // Decrementa o tempo
        update_display = true; // Sinaliza a necessidade de atualizar o display
        return true; // Continua o timer
    }

    timer_running = false;
    update_display = true;
    return false; // Para o timer
}

// Função para lidar com a interrupção dos botões A e B
void button_irq_handler(uint gpio, uint32_t events) {
    // Verifica se o botão A foi pressionado
    if (gpio == BUTTON_A){
        buttonA_pressed = true;
        static absolute_time_t last_press = 0;
    
        // Debounce
        if (absolute_time_diff_us(last_press, get_absolute_time()) < 100000) {     
            return;
        }
        last_press = get_absolute_time();

        if (timer_running) {
            cancel_repeating_timer(&timer); // Cancela timer se já estiver ativo
        }

        time_passed = 9; // Reinicia tempo
        num_pressed_buttonB = 0; // Zera contador do botão B
        timer_running = true;
        update_display = true;
    } 

    // Verifica se o botão B foi pressionado, se o timer está zerado e se o timer está ativo
    if (gpio == BUTTON_B && time_passed != 0 && timer_running){
        static absolute_time_t last_press = 0;
    
        // Debounce
        if (absolute_time_diff_us(last_press, get_absolute_time()) < 100000) {
            return;
        }
        last_press = get_absolute_time();

        num_pressed_buttonB++; // Incrementa contador do botão B
        update_display = true; 
    }
    
    
}

// Função para configuração da interrupção
void setup_interrupts() {
    // Associa o mesmo callback a ambos os botões com interrupção na borda de descida
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, button_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, button_irq_handler);

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
    static uint last_time = 0;
    static uint last_count = 0;
    
    // Verifica a necessidade de atualizar o display
    if (!update_display && (last_time == time_passed) && (last_count == num_pressed_buttonB)) {
        return;
    }
    
    // Atualização dos valores atuais para comparação futura
    last_time = time_passed;
    last_count = num_pressed_buttonB;
    update_display = false;

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
    sprintf(text, "%u %u", time_passed, num_pressed_buttonB);
    
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
int main() {
    // Inicializa a interface padrão de entrada/saída
    stdio_init_all();

    // Configuração dos periféricos
    setup_buttons();
    setup_interrupts();
    setup_oled();

    // Atualização do display OLED
    display_oled();

    // Loop principal do programa
    while (true) {
        // Verifica o pressionamento do botão A
        if (buttonA_pressed){
            add_repeating_timer_ms(-1000, repeating_timer_callback, NULL, &timer);
            buttonA_pressed = false;
        }
        // Verifica a necessidade de atualizar o display OLED
        if (update_display) {
            display_oled();
            update_display = false;
        }
    }
}
