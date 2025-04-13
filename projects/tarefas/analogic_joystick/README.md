# 🎮 Leitura do Joystick Analógico da BitDogLab via ADC no RP2040
<br>

## 🎯 Objetivo do projeto

Este projeto tem como objetivo implementar, em linguagem C, um sistema embarcado utilizando a BitDogLab, que realiza a leitura dos valores analógicos de um joystick através dos conversores analógico-digitais (ADC) do microcontrolador RP2040.  

As leituras são exibidas dinamicamente em um display OLED SSD1306 conectado via I2C.  

O projeto busca demonstrar na prática:

- Uso de entradas analógicas (ADC) no RP2040.
- Interpretação de sinais analógicos vindos de um joystick.
- Exibição em tempo real dos dados em um display I2C.
<br>

## 🔁 Fluxo de Execução

1. **O sistema é inicializado, configurando:**
   - Os canais ADC para leitura dos sinais analógicos.
   - A comunicação I2C com o display OLED. 

2. **A cada ciclo do loop:**
   - Os valores dos eixos X e Y do joystick são lidos.
   - Os valores são convertidos para números digitais de 0 a 4095.
   - As leituras são formatadas e exibidas no display OLED, centralizadas na tela.

3. **O display mostra continuamente:**
   - **Eixo X:** Valor atual do eixo horizontal do joystick.
   - **Eixo Y:** Valor atual do eixo vertical do joystick.
<br>

## 🧩 Componentes usados

- 1x Raspberry Pi Pico W  
- 1x Display OLED SSD1306 
- 1x Joystick Analógico
<br>

## ⚡ Pinagem dos dispositivos

| Componente        |Pino Pico     |Função                               |
|-------------------|--------------|-------------------------------------|
| VRx               |    GPIO27    |     Leitura analógica do eixo X     |
| VRy               |    GPIO26    |     Leitura analógica do eixo Y     |
| Display OLED SDA  |    GPIO14    |     Comunicação I2C                 |
| Display OLED SCL  |    GPIO15    |     Comunicação I2C                 |
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
