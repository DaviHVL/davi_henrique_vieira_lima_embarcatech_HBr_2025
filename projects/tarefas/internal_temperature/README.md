# 🌡️ Leitura da Temperatura Interna do RP2040 com Exibição em Display OLED
<br>

## 🎯 Objetivo do projeto

Este projeto tem como objetivo implementar, em linguagem C, um sistema embarcado utilizando a BitDogLab para realizar a leitura da temperatura interna do microcontrolador RP2040 por meio de seu sensor embutido.

Os valores da temperatura são convertidos para graus Celsius e exibidos dinamicamente em um display OLED SSD1306 conectado via I2C.

O projeto busca demonstrar na prática:

- Uso do sensor de temperatura interno do RP2040 com ADC.
- Conversão do sinal analógico para valor em graus Celsius.
- Exibição da temperatura em tempo real em um display OLED via I2C.
<br>

## 🔁 Fluxo de Execução

1. **O sistema é inicializado, configurando:**
   - O ADC interno para leitura do sensor de temperatura do chip.
   - A comunicação I2C com o display OLED SSD1306.

2. **A cada ciclo do loop:**
   - O valor da temperatura é lido em forma digital pelo ADC.
   - A leitura é convertida em tensão e, posteriormente, em graus Celsius.
   - A temperatura formatada é exibida no centro do display OLED.

3. **O display mostra continuamente:**
   - A mensagem `Temp XX XX`, onde `XX XX` representa a temperatura atual em Celsius, considerando duas casas decimais.
<br>

## 🧩 Componentes usados

- 1x Raspberry Pi Pico W  
- 1x Display OLED SSD1306 
<br>

## ⚡ Pinagem dos dispositivos

| Componente         | Pino Pico | Função                         |
|--------------------|-----------|---------------------------------|
| Sensor de Temperatura Interno |  -        |  Canal 4 do ADC interno         |
| Display OLED SDA   | GPIO14    | Comunicação I2C                |
| Display OLED SCL   | GPIO15    | Comunicação I2C                |
<br>

## 🧪 Como compilar e executar

1. **Configuração inicial**  
   Certifique-se de ter o **Raspberry Pi Pico SDK** corretamente instalado no seu sistema, assim como o **CMake** e uma estrutura de projeto compatível.

2. **Clone ou copie os arquivos do projeto**

3. **Monte a estrutura do projeto**

4. **Grave o arquivo `.uf2`** gerado na memória do Pico.

5. **Conecte o Pico via USB segurando o botão BOOTSEL**, monte o disco e copie o `.uf2`.
<br>

## 📈 Resultados esperados ou observados
É esperado o comportamento descrito em **🔁 Fluxo de Execução**