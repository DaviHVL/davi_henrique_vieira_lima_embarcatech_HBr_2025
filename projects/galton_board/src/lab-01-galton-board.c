// Inclusão de bibliotecas padrão
#include <stdio.h>      // Biblioteca padrão de entrada e saída 
#include <string.h>     // Biblioteca para manipulação de strings

// Bibliotecas específicas do Raspberry Pi Pico
#include "pico/stdlib.h"    // Funções básicas como inicialização de GPIOs, delays, etc.
#include "hardware/i2c.h"   // Controle do barramento I2C

// Biblioteca customizada para controlar o display OLED SSD1306
#include "include/ssd1306.h"

// ---------- Configurações de hardware e display ----------

// Define qual porta I2C será usada (no caso, i2c0)
#define I2C_PORT i2c0

// Define os pinos GPIO conectados ao SDA e SCL do display OLED
#define I2C_SDA 14
#define I2C_SCL 15

// Dimensões do display OLED (em pixels)
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// ---------- Parâmetros para o desenho do simulador de Galton ----------

// Número de linhas de pinos na simulação 
#define PIN_ROWS 8

// Número de compartimentos no histograma 
#define BIN_COUNT 16

// Altura máxima das barras do histograma (em pixels)
#define HISTOGRAM_MAX_BAR_HEIGHT 20

// Posição vertical inicial da primeira linha de pinos
#define PIN_START_Y 17

// Espaçamento vertical entre linhas de pinos (em pixels)
#define PIN_VERTICAL_SPACING 6

// Espaçamento horizontal entre pinos adjacentes (em pixels)
#define PIN_HORIZONTAL_SPACING 14

// Passo horizontal da bola a cada linha, define o quanto ela pode se mover para os lados
#define BALL_STEP_X (PIN_HORIZONTAL_SPACING / 2)

// ---------- Definições de botões de controle ----------

// GPIOs usados como botões de controle 
#define BUTTON_A 5
#define BUTTON_B 6

// ---------- Parâmetros de simulação ----------

// Número máximo de bolas em queda simultaneamente
#define MAX_BALLS 12

// Intervalo de tempo (em ciclos de loop) entre o surgimento de novas bolas
#define BALL_DROP_INTERVAL 5

// Número de bolas para encerrar a simulação e mostrar o histograma
#define HISTOGRAM_TRIGGER_COUNT 20

// Tendência de inclinação nas decisões da bola (0.5 = neutro, <0.5 favorece esquerda, >0.5 favorece direita)
#define BIAS 0.3

// Buffer que representa os pixels do display SSD1306 
uint8_t ssd_buffer[1024];

// Estrutura que define uma área de renderização
struct render_area frame_area;

// Estrutura que representa uma esfera na simulação de Galton
typedef struct Ball {
    uint16_t x;       // Posição horizontal da bola no display
    uint16_t y;       // Posição vertical da bola no display
    uint16_t vel;     // Velocidade da bola 
    bool active;      // Indica se a bola ainda está "em queda" 
} Ball;

// Bola usada na simulação de uma única esfera
Ball current_ball;

// Array de bolas usadas na simulação multi-esferas
Ball multiple_balls[MAX_BALLS];

// Contador para controlar o intervalo de tempo entre quedas de bolas na simulação multi-esferas
int ball_drop_timer = 0;

// Contador de bolas que caíram em cada compartimento (bin) do histograma
uint16_t balls_per_bin[BIN_COUNT] = {0};

// Número de bolas que terminaram a simulação desde a última exibição do histograma
int balls_finished_since_last_histogram = 0;

// Cálculo do espaçamento vertical entre linhas de pinos com base na altura do display
int vertical_spacing = DISPLAY_HEIGHT / (PIN_ROWS + 1);

// Cálculo do espaçamento horizontal entre pinos com base na largura do display
int horizontal_spacing = DISPLAY_WIDTH / (PIN_ROWS + 1);

// Indica se o histograma deve ser exibido na tela
bool show_histogram = false;

// Indica se o menu inicial já foi concluído 
volatile bool menu_complete = false;

// Modo da simulação: verdadeiro se for multi-esferas, falso se for bola única
volatile bool multiball_mode = false;

// Contador total de bolas que já terminaram a simulação 
int finished_balls = 0;

// Define se o viés (bias) está ativado; quando ativado, a bola tende mais para um lado
bool bias_enabled = false;

// Indica se a simulação já foi iniciada 
bool simulation_started = false;

// Função de Aleatoriedade Binária
// Retorna true ou false aleatoriamente, com ou sem viés dependendo do valor de bias_enabled
bool random_bin(){
    if (!bias_enabled){
        // Sem viés: 50% de chance para cada lado
        int num_ref = rand();
        return (num_ref % 2 == 0);
    } else {
        // Com viés: probabilidade controlada pela constante BIAS
        return (rand() / (double)RAND_MAX) < BIAS ? true : false;
    }
}

// Função que atualiza o contador de bolas no compartimento (bin) correspondente
// bin_index: índice do compartimento em que a bola caiu
void update_balls_per_bin(int bin_index){
    if (bin_index >= 0 && bin_index < BIN_COUNT) {
        balls_per_bin[bin_index] += 1;
    }
}

// Contador global de bolas já iniciadas na simulação
uint16_t total_balls = 0;

// Função que inicializa uma bola
// Define sua posição inicial no centro, sua velocidade e marca como ativa
void init_ball(Ball* ball){
    total_balls += 1;
    ball->y = 0;                         // Começa no topo do display
    ball->x = DISPLAY_WIDTH / 2;        // Centralizada horizontalmente
    ball->vel = 1;                      // Velocidade padrão
    ball->active = true;               // Marca a bola como "em queda"
}

// Função para atualizar a posição da esfera a cada ciclo
// Responsável pela simulação do movimento da bola pelas fileiras de pinos e pela coleta no bin
void update_position(Ball* ball){
    // Move a bola para baixo com base na velocidade
    ball->y += ball->vel;

    // Verifica se a bola chegou na parte inferior do display
    if (ball->y >= DISPLAY_HEIGHT - 1){
        // Calcula a largura de cada compartimento
        const float bin_width = (float)DISPLAY_WIDTH / BIN_COUNT;
    
        // Calcula o índice do compartimento com arredondamento proporcional
        int bin_index = (int)((ball->x + bin_width/2) / bin_width);
    
        // Garante que o índice esteja dentro dos limites válidos
        bin_index = (bin_index >= BIN_COUNT) ? BIN_COUNT - 1 : 
            (bin_index < 0) ? 0 : bin_index;

        if (!multiball_mode){
            // Modo de uma única bola: reinicia a simulação imediatamente após a bola cair
            update_balls_per_bin(bin_index);
            show_histogram = true;      // Exibe histograma após cada queda
            init_ball(ball);            // Reinicializa a bola
        } else {
            // Modo multi-bola: apenas desativa a bola e conta finalizadas
            finished_balls += 1;
            update_balls_per_bin(bin_index);
            ball->active = false;

            // Após um número fixo de bolas finalizadas, exibe o histograma
            if (finished_balls == 50){
                show_histogram = true;
                finished_balls = 0;
            }
        }

        return; // Encerrar a função após o tratamento da bola na base
    }

    // Verifica se a bola passou por alguma linha de pinos
    for (int r = 0; r < PIN_ROWS; ++r) {
        uint16_t pin_row_y = PIN_START_Y + (r * PIN_VERTICAL_SPACING);

        // Se a bola cruzou a linha vertical do pino, decide direção aleatória
        if ((ball->y >= pin_row_y) && ((ball->y - ball->vel) < pin_row_y)) {
            if (random_bin()) {
                ball->x += BALL_STEP_X;  // Move para a direita
            } else {
                ball->x -= BALL_STEP_X;  // Move para a esquerda
            }

            // Limita a posição da bola dentro da largura do display
            if (ball->x >= ssd1306_width) ball->x = ssd1306_width - 1;
            if (ball->x < 0) ball->x = 0;

            break; // Sai do loop após mover a bola uma vez por linha de pinos
        }
    }

    return;
}

// Função para configuração dos botões
void setup_buttons() {
    gpio_init(BUTTON_A); // Inicialização do pino 
    gpio_set_dir(BUTTON_A, GPIO_IN); // Definição do pino como entrada
    gpio_pull_up(BUTTON_A); // Ativação do pull-up

    gpio_init(BUTTON_B); // Inicialização do pino 
    gpio_set_dir(BUTTON_B, GPIO_IN); // Definição do pino como entrada
    gpio_pull_up(BUTTON_B); // Ativação do pull-up
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
    memset(ssd_buffer, 0, ssd1306_buffer_length);

    // Inicializa uma esfera
    init_ball(&current_ball);
}

// Função responsável por atualizar e exibir os conteúdos no display OLED
void display_oled(){
    // Define a área completa de renderização na tela OLED
    struct render_area frame_area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    // Calcula o tamanho do buffer com base na área a ser desenhada
    calculate_render_area_buffer_length(&frame_area);

    // Limpa o buffer da tela
    memset(ssd_buffer, 0, sizeof(ssd_buffer)); 

    // Se o menu ainda não foi finalizado, desenha o menu
    if (!menu_complete){
        draw_menu_on_display(ssd_buffer);
        render_on_display(ssd_buffer, &frame_area);
        sleep_ms(250); // Pausa breve para evitar flickering
    } else {
        if (show_histogram){ 
            // Se for hora de mostrar o histograma, desenha-o e exibe na tela
            draw_histogram_on_display(ssd_buffer);
            render_on_display(ssd_buffer, &frame_area);
            show_histogram = false;
            sleep_ms(2000); // Tempo para o usuário visualizar o histograma
        } else {
            // Desenha os pinos na tela
            draw_pins_on_display(ssd_buffer);

            if (multiball_mode) {
                // Controle de tempo para soltar novas bolas no modo multibola
                ball_drop_timer++;
                if (ball_drop_timer >= BALL_DROP_INTERVAL) {
                    ball_drop_timer = 0;
                    // Inicializa uma nova bola inativa, se houver
                    for (int i = 0; i < MAX_BALLS; ++i) {
                        if (!multiple_balls[i].active) {
                            init_ball(&multiple_balls[i]);
                            break; // Ativa uma bola por vez
                        }
                    }
                }

                // Atualiza e desenha todas as bolas ativas
                for (int i = 0; i < MAX_BALLS; ++i) {
                    if (multiple_balls[i].active) {
                        update_position(&multiple_balls[i]);
                        draw_ball_on_display(ssd_buffer, &multiple_balls[i]);
                    }
                }
            } else {
                // Modo de bola única: atualiza e desenha apenas uma bola
                update_position(&current_ball);
                draw_ball_on_display(ssd_buffer, &current_ball);
            }

            // Mostra o contador total de bolas
            draw_ball_counter_on_display(ssd_buffer);

            // Renderiza o frame completo no display
            render_on_display(ssd_buffer, &frame_area);
            sleep_ms(50); // Tempo para manter fluidez da animação
        }
    }
}

// Função para desenhar a esfera na tela com 3 pixels
void draw_ball_on_display(uint8_t* buf, const Ball* ball) {
    if (ball->y < ssd1306_height && ball->x >= 0 && ball->x < ssd1306_width) {
        ssd1306_set_pixel(buf, ball->x + 1, ball->y, true);
        ssd1306_set_pixel(buf, ball->x, ball->y + 1, true);
        ssd1306_set_pixel(buf, ball->x + 1, ball->y + 1, true);
    }
}

// Função que desenha os pinos na tela, distribuídos em linhas
void draw_pins_on_display(uint8_t* buf) {
    for (int r = 0; r < PIN_ROWS; ++r) {
        uint16_t pin_row_y = PIN_START_Y + (r * PIN_VERTICAL_SPACING);
        if (pin_row_y >= ssd1306_height) continue;

        int pins_in_this_row = r + 1;
        uint16_t width_of_all_pins_in_row = (pins_in_this_row <= 1) ? 0 : (pins_in_this_row - 1) * PIN_HORIZONTAL_SPACING;
        uint16_t start_x_for_row = (ssd1306_width - width_of_all_pins_in_row) / 2;

        for (int p = 0; p < pins_in_this_row; ++p) {
            uint16_t pin_x = start_x_for_row + (p * PIN_HORIZONTAL_SPACING);
            if (pin_x < ssd1306_width && pin_x >= 0) {
                ssd1306_set_pixel(buf, pin_x, pin_row_y, true);
            }
        }
    }
}

// Função que mostra o número total de bolas lançadas na simulação
void draw_ball_counter_on_display(uint8_t* buf) {
    char counter_text[20];
    sprintf(counter_text, "Total %lu", total_balls);
    ssd1306_draw_string(buf, 0, 0, counter_text);
}

// Função que desenha o histograma com base nas bolas coletadas em cada bin
void draw_histogram_on_display(uint8_t* buf) {
    char counter_text[20];
    sprintf(counter_text, "Histograma");
    ssd1306_draw_string(buf, 0, 0, counter_text);

    int collector_width = ssd1306_width / BIN_COUNT;

    // Determina o maior valor entre os bins para escalar as barras
    uint16_t max_balls_in_bin_val = 1;
    for (int i = 0; i < BIN_COUNT; ++i) {
        if (balls_per_bin[i] > max_balls_in_bin_val) {
            max_balls_in_bin_val = balls_per_bin[i];
        }
    }

    // Desenha as barras verticais para cada bin proporcionalmente
    for (int i = 0; i < BIN_COUNT; ++i) {
        uint16_t bar_height = (balls_per_bin[i] * HISTOGRAM_MAX_BAR_HEIGHT) / max_balls_in_bin_val;
        if (bar_height > HISTOGRAM_MAX_BAR_HEIGHT) bar_height = HISTOGRAM_MAX_BAR_HEIGHT;
        if (bar_height == 0 && balls_per_bin[i] > 0) bar_height = 1;

        uint16_t bar_x_start = i * collector_width;

        for (uint16_t h = 0; h < bar_height; ++h) {
            for (uint16_t w = 0; w < collector_width - 1; ++w) {
                if (bar_x_start + w < ssd1306_width) {
                    ssd1306_set_pixel(buf, bar_x_start + w, (ssd1306_height - 1) - h, true);
                }
            }
        }
    }
}

// Função que desenha o menu inicial com instruções
void draw_menu_on_display(uint8_t* buf) {
    ssd1306_draw_string(buf, 10, 10, "Galton Board");
    ssd1306_draw_string(buf, 10, 25, "Press A: Sing");
    ssd1306_draw_string(buf, 10, 35, "Press B: Mult");
    ssd1306_draw_string(buf, 10, 45, "Press A After:");
    ssd1306_draw_string(buf, 10, 55, "For BIAS");
}

// Variável estática para armazenar o tempo do último acionamento válido do Botão A
static uint32_t last_button_A_press_time = 0;

// Função de interrupção chamada quando um botão é pressionado
// Define o modo de simulação com base no botão (A ou B)
// Implementa debounce para o Botão A.
void button_irq_handler(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A) {
        uint32_t now = time_us_32(); 

        if ((now - last_button_A_press_time) < (150 * 1000)) {
            return;
        }

        last_button_A_press_time = now;

        if (!menu_complete) { 
             menu_complete = true;
        }

        if (!simulation_started) {
            multiball_mode = false;
            simulation_started = true;
        } else {
            if (!bias_enabled) { 
                bias_enabled = true;

                gpio_set_irq_enabled(BUTTON_A, GPIO_IRQ_EDGE_FALL, false);
            }
        }
    } else if (gpio == BUTTON_B) {
        if (!menu_complete) {
            menu_complete = true;
        }
        
        multiball_mode = true;
        simulation_started = true;

        gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, false);
    }
}

// Configura as interrupções dos botões A e B
void setup_interrupt() {
    // Habilita interrupções para os botões na borda de descida
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, button_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, button_irq_handler);
}

int main() {
    stdio_init_all();   // Inicializa os sistemas de entrada e saída (stdout, stderr, etc.)
    setup_oled();       // Inicializa o display OLED
    setup_buttons();    // Configura os botões para serem usados no programa
    setup_interrupt();  // Configura as interrupções associadas aos botões

    while (true) {      // Laço principal que irá rodar indefinidamente
        display_oled(); // Exibe o conteúdo na tela OLED
    }
}