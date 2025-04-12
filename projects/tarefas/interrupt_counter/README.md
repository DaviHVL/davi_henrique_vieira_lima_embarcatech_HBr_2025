# ⏱️ Contador Decrescente com Registro de Eventos via Interrupção
<br>

## 🎯 Objetivo do projeto

Este projeto tem como objetivo implementar, em linguagem C, um sistema embarcado utilizando a **BitDogLab**, onde um **contador decrescente** é controlado por **interrupções externas** (através de botões). Durante a contagem, o sistema também registra eventos (cliques) em um segundo botão, exibindo todas as informações em um **display OLED SSD1306**.

O projeto busca demonstrar na prática:

- Uso de interrupções GPIO.
- Manipulação de timers com contagem regressiva.
- Registro de eventos durante janelas de tempo específicas.
- Exibição dinâmica de informações em um display via comunicação I2C.
<br>
## 🔁 Fluxo de Execução

1. **Pressionando o Botão A:**
   - Reinicia o contador para **9**.
   - Inicia a contagem regressiva (1 segundo por unidade).
   - O display OLED exibe:
     - O valor atual do contador.
     - A quantidade de vezes que o **Botão B** foi pressionado.

2. **Durante a contagem (9 a 0):**
   - A cada segundo, o contador decrementa automaticamente.
   - Cada clique no **Botão B** é registrado.
   - A interface é atualizada em tempo real com ambos os valores.

3. **Quando o contador atinge zero:**
   - O sistema **congela**.
   - Cliques no **Botão B são ignorados**.
   - O display permanece exibindo:
     - O valor **0** do contador.
     - A **quantidade total de cliques** no botão B durante os 9 segundos de contagem.

4. **Para reiniciar:**
   - Pressione novamente o **Botão A**.
   - O contador volta para **9**.
   - O contador de cliques é **zerado**.
   - O processo se repete.
<br>
## 🧩 Componentes usados

- 1x Raspberry Pi Pico W
- 1x Display OLED SSD1306 (I2C)
- 2x Push-Buttons
<br>
## ⚡ Pinagem dos dispositivos

| Componente       | Pino Pico | Função                |
|------------------|-----------|------------------------|
| Botão A          | GPIO5     | Inicia/Reinicia contagem |
| Botão B          | GPIO6     | Registra cliques durante contagem |
| Display OLED SDA | GPIO14    | Comunicação I2C        |
| Display OLED SCL | GPIO15    | Comunicação I2C        |
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
