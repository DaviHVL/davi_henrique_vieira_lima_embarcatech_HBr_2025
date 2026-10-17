// Bibliotecas necessárias
#include <stdio.h> // Biblioteca padrão
#include <math.h> // Biblioteca de matemática (função "round" foi utilizada)
#include <string.h> // Biblioteca de manipulação de strings

#include "pico/stdlib.h" // Biblioteca padrão pico
#include "hardware/gpio.h" // Biblioteca de GPIOs
#include "hardware/adc.h" // Biblioteca do ADC
#include "hardware/pwm.h" // Biblioteca do PWM
#include "hardware/dma.h" // Biblioteca do DMA
#include "inc/ssd1306.h"  // Biblioteca do display OLED SSD1306
#include "inc/ssd1306_i2c.h" // Biblioteca de I2C do SSD1306
// ----------------------------------------------------------------------------------------------------

// Definições de constantes
#define BUTTON_A 5 // Pino do Botão A
#define BUTTON_B 6 // Pino do Botão B

#define MIC_CHANNEL 2 // Canal ADC do microfone
#define MIC_ADC_PIN 28 // Pino do ADC
#define ADC_CLOCK_DIV 6000.0f // Divisor de clock do ADC
#define SAMPLES 40000 // Número de amostras 
#define ADC_RESOLUTION (1 << 12)  // Resolução do ADC (12-bit ADC -> 4096 níveis)
#define V_REF 3.3f  // Tensão de referência do ADC

#define LED_RGB_GREEN 11 // Pino do LED RGB Verde
#define LED_RGB_RED 13 // Pino do LED RGB Vermelho
#define LED_RGB_BLUE 12 // Pino do LED RGB Azul

// Define os pinos GPIO conectados ao SDA e SCL do display OLED
#define I2C_SDA 14
#define I2C_SCL 15

// Dimensões do display OLED (em pixels)
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

#define SAMPLES_PER_COLUMN 950 // Quantidade de amostras para calcular cada coluna vertical do display

#define BUZZER_PIN 21 // Pino do buzzer
// ----------------------------------------------------------------------------------------------------

// Definição das variáveis globais
uint dma_channel; // Canal DMA
dma_channel_config dma_cfg; // Configuração do DMA
uint16_t adc_buffer[SAMPLES]; // Buffer de armazenamento de todas amostras de ADC

uint16_t adc_capture_buffer[SAMPLES_PER_COLUMN]; // Buffer para capturar amostras do ADC
uint16_t dc_offset = 2048; // Um valor inicial padrão para o offset DC (metade de 12-bit)

uint8_t ssd[ssd1306_buffer_length]; // Buffer do display OLED

struct render_area frame_area; // Estrutura que define uma área de renderização

bool is_recording_or_playing = false; // Indica se o dispositivo está gravando ou reproduzindo
// ----------------------------------------------------------------------------------------------------

// Funções de configuração
// Função para configuração dos botões
void setup_buttons() {
    gpio_init(BUTTON_A); // Inicialização do pino 
    gpio_set_dir(BUTTON_A, GPIO_IN); // Definição do pino como entrada
    gpio_pull_up(BUTTON_A); // Ativação do pull-up

    gpio_init(BUTTON_B); // Inicialização do pino 
    gpio_set_dir(BUTTON_B, GPIO_IN); // Definição do pino como entrada
    gpio_pull_up(BUTTON_B); // Ativação do pull-up
}

// Função para configuração do LED RGB
void setup_rgb_led() {
  gpio_init(LED_RGB_GREEN); // Inicialização do pino do LED RGB Verde
  gpio_set_dir(LED_RGB_GREEN, GPIO_OUT); // Definição do pino como saída

  gpio_init(LED_RGB_RED); // Inicialização do pino do LED RGB Vermelho
  gpio_set_dir(LED_RGB_RED, GPIO_OUT); // Definição do pino como saída

  gpio_init(LED_RGB_BLUE); // Inicialização do pino do LED RGB Azul
  gpio_set_dir(LED_RGB_BLUE, GPIO_OUT); // Definição do pino como saída
}

// Função para configuração do ADC
void setup_adc_and_display(){
  adc_gpio_init(MIC_ADC_PIN); // Inicialização do pino como ADC
  adc_init(); // Inicialização do módulo ADC

  adc_select_input(MIC_CHANNEL); // Seleção do canal de entrada do ADC
  adc_fifo_setup(true, true, 1, false, false); // Configuração do FIFO
  adc_set_clkdiv(ADC_CLOCK_DIV); // Definição do divisor de Clock

  // Configuração do display OLED
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

// Função para configuração do DMA para obteção de dados do ADC
void setup_dma(){
  // Requisição de um canal DMA disponível
  dma_channel = dma_claim_unused_channel(true);

  // Configuração padrão para o canal DMA 
  dma_cfg = dma_channel_get_default_config(dma_channel);

  // Definição do tamanho da transferência de dados (16 bits)
  channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16);

  // Desativação do incremento do endereço de leitura
  channel_config_set_read_increment(&dma_cfg, false);

  // Ativação do incremento do endereço de escrita 
  channel_config_set_write_increment(&dma_cfg, true);

  // Definição do gatilho da transferência DMA como o ADC
  channel_config_set_dreq(&dma_cfg, DREQ_ADC);
}

// Função para configuração do PWM do buzzer
void setup_pwm(){
  // Identifica qual "slice" de PWM está conectado ao pino do buzzer
  uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);

  // Configura o pino do buzzer para a função de PWM
  gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM); 
  
  // Define o divisor de clock do PWM. Um valor baixo como 1.0f aumenta a frequência 
  // do PWM para fora da faixa audível, melhorando a qualidade do som.
  pwm_set_clkdiv(slice, 1.0f);  

  // Define o valor de "wrap". Este é o valor máximo que o contador do PWM atinge.
  // Usamos (ADC_RESOLUTION - 1), ou seja, 4095, para que a resolução do PWM
  // seja a mesma do ADC (12 bits). Isso simplifica a reprodução.
  pwm_set_wrap(slice, ADC_RESOLUTION - 1);

  // Define o nível inicial do canal (duty cycle) para 0.
  pwm_set_chan_level(slice, pwm_gpio_to_channel(BUZZER_PIN), 0);

  // Ativa o PWM
  pwm_set_enabled(slice, true);
}
// ----------------------------------------------------------------------------------------------------

// Funções de manipulação do display e processamento de dados
// Função para deslocamento no display oled
void shift_display_buffer_left(uint8_t* ssd) {
  // O buffer do SSD1306 é organizado em "páginas" de 8 pixels de altura.
  // Para rolar a tela, precisamos deslocar cada página individualmente.
  for (int page = 0; page < (DISPLAY_HEIGHT / 8); page++) {
    uint8_t* page_ptr = ssd + (page * DISPLAY_WIDTH);
    // Move os dados da (coluna + 3) para a (coluna), efetivamente deslocando para a esquerda
    memmove(page_ptr, page_ptr + 3, DISPLAY_WIDTH - 3);
    // Limpa a última coluna para que novos dados possam ser desenhados
    page_ptr[DISPLAY_WIDTH - 1] = 0x00;
  }
}

// Função para atualizar a forma de onda no display
void update_scrolling_waveform(uint8_t* ssd) {
  // Desloca o buffer do display para a esquerda para abrir espaço para a nova coluna
  shift_display_buffer_left(ssd);

  uint16_t min_val = 4095;
  uint16_t max_val = 0;

  // Encontra os valores mínimo e máximo no bloco para determinar a amplitude
  for (int i = 0; i < SAMPLES_PER_COLUMN; i++) {
    if (adc_capture_buffer[i] < min_val){
      min_val = adc_capture_buffer[i];
    } 
    if (adc_capture_buffer[i] > max_val){
      max_val = adc_capture_buffer[i];
    } 
  }

  // Suaviza a variação do offset DC para estabilizar a onda no centro
  long current_dc_sum = 0;
  for(int i=0; i<SAMPLES_PER_COLUMN; i++) current_dc_sum += adc_capture_buffer[i];
  uint16_t current_dc = current_dc_sum / SAMPLES_PER_COLUMN;
  dc_offset = (dc_offset * 0.95) + (current_dc * 0.05);

  // Remove o offset DC para centralizar a onda
  int16_t centered_min = min_val - dc_offset;
  int16_t centered_max = max_val - dc_offset;
    
  const float amplification_factor = 2.5f; // Fator de amplificação para melhor visualização

  // Mapeia a amplitude para a altura do display
  int y_start = (int)((float)centered_min * amplification_factor * (float)DISPLAY_HEIGHT / (float)ADC_RESOLUTION);
  int y_end = (int)((float)centered_max * amplification_factor * (float)DISPLAY_HEIGHT / (float)ADC_RESOLUTION);

  // Centraliza a linha vertical na tela
  y_start = (DISPLAY_HEIGHT / 2) - y_start;
  y_end = (DISPLAY_HEIGHT / 2) - y_end;

  // Garante que os valores de Y fiquem dentro dos limites do display
  if (y_start < 0){
    y_start = 0; // Garante que y_start não seja negativo
  } 
  if (y_start >= DISPLAY_HEIGHT){
    y_start = DISPLAY_HEIGHT - 1; // Garante que y_start não ultrapasse a altura do display
  } 
  if (y_end < 0){
    y_end = 0; // Garante que y_end não seja negativo
  }
  if (y_end >= DISPLAY_HEIGHT){
    y_end = DISPLAY_HEIGHT - 1; // Garante que y_end não ultrapasse a altura do display
  } 
  if (y_start > y_end) { // Garante que y_start seja sempre menor
    int temp = y_start;
    y_start = y_end;
    y_end = temp;
  }

  // A posição X é fixa na última coluna (extrema direita)
  int x_pos = DISPLAY_WIDTH - 1;
    
  // Preenchimenta a coluna vertical no buffer do display
  for (int y = y_start; y <= y_end; y++) {
    // Usa a função `ssd1306_set_pixel` que você forneceu
    ssd1306_set_pixel(ssd, x_pos, y, true);
  }

  // Desenha uma linha vertical na última coluna do display
  ssd1306_send_buffer(ssd, ssd1306_buffer_length);
}

// Função para capturar amostras do ADC e exibir a forma de onda no display
void capture_and_display_loop(uint8_t* ssd) {
  // Limpa o buffer do display
  memset(ssd, 0, ssd1306_buffer_length);

  // Total de amostras capturadas
  size_t total_samples_captured = 0;

  // Inicialização do ADC e DMA
  while (total_samples_captured + SAMPLES_PER_COLUMN <= SAMPLES) {
    adc_fifo_drain();
    adc_run(false);

    dma_channel_configure(dma_channel, &dma_cfg,
                            adc_capture_buffer,
                            &adc_hw->fifo,
                            SAMPLES_PER_COLUMN,
                            true);
    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_channel);
    adc_run(false);

    // Atualiza a área de renderização do display
    update_scrolling_waveform(ssd);

    // Copia os dados para adc_buffer
    for (int i = 0; i < SAMPLES_PER_COLUMN; i++) {
      adc_buffer[total_samples_captured + i] = adc_capture_buffer[i];
    }

    // Atualiza o número total de amostras capturadas
    total_samples_captured += SAMPLES_PER_COLUMN;
    }
}

// Função para desligar os LEDs RGB
void turn_off_rgb_leds() {
  gpio_put(LED_RGB_GREEN, false); // Desliga o LED RGB Verde
  gpio_put(LED_RGB_RED, false); // Desliga o LED RGB Vermelho
  gpio_put(LED_RGB_BLUE, false); // Desliga o LED RGB Azul
}

// Função para reproduzir as amostras capturadas no buzzer
void play_samples_on_buzzer() {
  // Buffers para processamento. Usamos um buffer intermediário para o filtro.
  int16_t signed_buffer[SAMPLES];
  int16_t filtered_buffer[SAMPLES];

  // Cálculo do offset DC
  long sum = 0;
  for (int i = 0; i < SAMPLES; i++) {
    sum += adc_buffer[i];
  }
  uint16_t dc_offset = sum / SAMPLES;

  // Remoção do offset DC de todas as amostras, criando um buffer com sinal
  for (int i = 0; i < SAMPLES; i++) {
    signed_buffer[i] = (int16_t)adc_buffer[i] - dc_offset;
  }

  // Aplicação do filtro de média móvel para remover ruído
  const int window_size = 9; 
  for (int i = 0; i < SAMPLES; i++) {
    long sum_filter = 0;
    int count = 0;
    // Loop da janela do filtro
    for (int j = -window_size / 2; j <= window_size / 2; j++) {
      if (i + j >= 0 && i + j < SAMPLES) {
        sum_filter += signed_buffer[i + j];
        count++;
      }
    }
    if (count > 0) {
      filtered_buffer[i] = sum_filter / count;
    } else {
      filtered_buffer[i] = signed_buffer[i]; // Borda, caso raro
    }
  }

  // Aplicação do ganho e reprodução do buffer JÁ FILTRADO
  const float gain = 7.0f;
  uint slice = pwm_gpio_to_slice_num(BUZZER_PIN);
  uint channel = pwm_gpio_to_channel(BUZZER_PIN);
  const int delay_us = 130; 

  for (int i = 0; i < SAMPLES; i++) {
    int32_t sample_ac = filtered_buffer[i];

    sample_ac = round(sample_ac * gain);

    const int16_t max_amplitude = (ADC_RESOLUTION / 2) - 1;
    if (sample_ac > max_amplitude){
      sample_ac = max_amplitude;
    } 
    if (sample_ac < -max_amplitude){
      sample_ac = -max_amplitude;
    } 

    int pwm_level = (ADC_RESOLUTION / 2) + sample_ac;

    if (pwm_level < 0){
      pwm_level = 0;
    } 
    if (pwm_level > ADC_RESOLUTION - 1){
      pwm_level = ADC_RESOLUTION - 1;
    } 

    pwm_set_chan_level(slice, channel, pwm_level);
    sleep_us(delay_us);
  }

  // Silenciamento do buzzer no final
  pwm_set_chan_level(slice, channel, (ADC_RESOLUTION / 2));
  sleep_ms(1);
  pwm_set_chan_level(slice, channel, 0);
}
// ----------------------------------------------------------------------------------------------------

// Função principal
int main() {
  // Inicializa a interface padrão de entrada/saída
  stdio_init_all();

  // Configuração dos periféricos
  setup_buttons();
  setup_adc_and_display();
  setup_dma();
  setup_rgb_led();
  setup_pwm();

  // Inicialização da mensagem inicial  no display OLED
  memset(ssd, 0, ssd1306_buffer_length);
  ssd1306_draw_string(ssd, 5, 28, "Pressione A");
  ssd1306_draw_string(ssd, 5, 33, "para gravar");
  ssd1306_send_buffer(ssd, ssd1306_buffer_length);

  // Loop principal
  while(true){
    // Verifica se o botão A foi pressionado
    if(!gpio_get(BUTTON_A)){
      if (!is_recording_or_playing) {
        sleep_ms(100); // Debounce delay
        is_recording_or_playing = true;
        gpio_put(LED_RGB_GREEN, true); // Liga o LED RGB Verde
        gpio_put(LED_RGB_RED, false); // Desliga o LED RGB Vermelho
        gpio_put(LED_RGB_BLUE, false); // Desliga o LED RGB Azul
      }
      capture_and_display_loop(ssd); // Captura e exibe a forma de onda

      // Indica o fim da captura
      memset(ssd, 0, ssd1306_buffer_length);
      ssd1306_draw_string(ssd, 5, 10, "Fim da Captura");
      ssd1306_draw_string(ssd, 5, 38, "Pressione B");
      ssd1306_draw_string(ssd, 5, 43, "para reproduzir");
      ssd1306_send_buffer(ssd, ssd1306_buffer_length);
    }
    is_recording_or_playing = false;

    // Verifica se o botão B foi pressionado
    if(!gpio_get(BUTTON_B)){
      if (!is_recording_or_playing) {
        sleep_ms(100); // Debounce delay
        is_recording_or_playing = true;
        gpio_put(LED_RGB_GREEN, false); // Desliga o LED RGB Verde
        gpio_put(LED_RGB_RED, true); // Liga o LED RGB Vermelho
        gpio_put(LED_RGB_BLUE, false); // Desliga o LED RGB Azul
      }
      // Indica que o dispositivo está reproduzindo o áudio
      memset(ssd, 0, ssd1306_buffer_length);
      ssd1306_draw_string(ssd, 5, 20, "Reproduzindo...");
      ssd1306_send_buffer(ssd, ssd1306_buffer_length);
      play_samples_on_buzzer();

      // Indica o fim da reprodução
      memset(ssd, 0, ssd1306_buffer_length);
      ssd1306_draw_string(ssd, 0, 15, "Fim da");
      ssd1306_draw_string(ssd, 0, 20, "Reproducao");
      ssd1306_draw_string(ssd, 0, 38, "Pressione A");
      ssd1306_draw_string(ssd, 0, 43, "para gravar");
      ssd1306_send_buffer(ssd, ssd1306_buffer_length);
    }
    is_recording_or_playing = false;

    turn_off_rgb_leds();
  }
  return 0;
}