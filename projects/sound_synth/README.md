
# Projetos de Sistemas Embarcados - EmbarcaTech 2025

Autor: **Davi Henrique Vieira Lima**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, Junho de 2025

---
<br>

# 🚀 Sintetizador de Áudio

Projeto orientado de sistemas embarcados com foco em áudio digital, utilizando a BitDogLab para **capturar, armazenar, processar e reproduzir** sinais de áudio.

---
<br>

## 🎯 Objetivo do projeto

Desenvolver um sintetizador de áudio embarcado que seja capaz de:
- Gravar um áudio de duração pré-definida via ADC.
- Armazenar as amostras em um buffer na memória RAM.
- Reproduzir o áudio capturado utilizando sinal PWM.
- Controlar as funções de gravação e reprodução com os botões da BitDogLab.
- Fornecer retorno visual utilizando LED RGB e display OLED.

---
<br>

## 🧩 Componentes usados

- **Raspberry Pi Pico**
- **Microfone de Eletreto**
- **Buzzer**
- **LED RGB**
- **Display OLED SSD1306**
- **Dois Push-Buttons**

---
<br>

## ⚡ Pinagem dos dispositivos

| Componente        | Pino na BitDogLab |
|-------------------|-------------------|
| Microfone         | GPIO 28                |
| Buzzer            | GPIO 21                |
| Botão A           | GPIO 5                 |
| Botão B           | GPIO 6                 |
| LED RGB           | GPIO 11, 12 e 13          |
| Display OLED (I2C)| SDA = GPIO 14 e SCL = GPIO 15|

---
<br>

## 🧪 Como compilar e executar

1. **Configuração inicial**  
   Certifique-se de ter o **Raspberry Pi Pico SDK** corretamente instalado no seu sistema, assim como o **CMake** e uma estrutura de projeto compatível.

2. **Clone ou copie os arquivos do projeto**

3. **Monte a estrutura do projeto**

4. **Grave o arquivo `.uf2`** gerado na memória do Pico.

5. **Conecte o Pico via USB segurando o botão BOOTSEL**, monte o disco e copie o `.uf2`.

---
<br>

## 📈 Resultados esperados ou observados

Ao executar o código, espera-se o seguinte comportamento:

1.  **Inicialização do Sistema**:
    * Após energizado, o sistema inicializa todos os periféricos: botões, ADC (com display), DMA, LED RGB e PWM. 
    * Indicando que o sistema está pronto para iniciar a captura de áudio, o display OLED mostrará a mensagem:
    ```
    Pressione A
    para gravar
    ```


2.  **Gravação de Áudio (Botão A)**:

    Ao pressionar o **Botão A**:
    * O sistema inicia a captura de dados de áudio via ADC com suporte de DMA.
    * O LED **Verde** acende indicando que o sistema está em modo de **gravação**.
    * A forma de onda do sinal de áudio é exibida em tempo real no display OLED.
    * Após a captura, o display mostra:
    ```
    Fim da Captura
    Pressione B
    para reproduzir
    ```
    * O LED Verde é desligado após o fim da captura.

3.  **Reprodução de Áudio (Botão B)**:

    Ao pressionar o **Botão B**:
    * O LED **Vermelho** acende indicando que o sistema está em modo de **reprodução**.
    * O display mostra a mensagem:
    ```
    Reproduzindo
    ```
    * Os dados capturados são processados e enviados via PWM para um buzzer, reproduzindo o som.
    * Após o término da reprodução, o display mostra:
    ```
    Fim da
    Reproducao
    Pressione A
    para gravar
    ```
    * O LED Vermelho é desligado.

4.  **Ciclo Contínuo**:

    O sistema permanece em loop aguardando novas interações. Os LEDs RGB e mensagens no display fornecem feedback visual ao usuário sobre o estado atual da aplicação.

---
<br>

## 📜 Licença
GNU GPL-3.0.

