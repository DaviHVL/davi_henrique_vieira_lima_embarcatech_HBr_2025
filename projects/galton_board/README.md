
# Projetos de Sistemas Embarcados - EmbarcaTech 2025

Autor: **Davi Henrique Vieira Lima**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, maio de 2025
<br>

# 🚀 Galton Board

Simulação de uma Galton Board utilizando um Raspberry Pi Pico e um display OLED SSD1306. O projeto demonstra visualmente como a aleatoriedade pode gerar um padrão estatístico, representado por uma distribuição normal à medida que o número de "bolas" aumenta (Lei dos Grandes Números).
<br>

## 🎯 Objetivo do projeto

O objetivo principal é criar uma simulação interativa da Galton Board na placa BitDogLab, exibindo o processo em tempo real em um display OLED. O projeto visa:

* Implementar a lógica de queda das bolas e suas interações com os pinos.
* Permitir a escolha entre simulação de bola única ou múltiplas bolas.
* Incluir uma opção para introduzir um viés (bias) na trajetória das bolas.
* Exibir um histograma da distribuição das bolas nos coletores.
<br>

## 🧩 Componentes usados

* Raspberry Pi Pico
* Display OLED SSD1306 
* 2x Push Buttons
<br>

## ⚡ Pinagem dos dispositivos
* **SDA (Serial Data)**: GPIO14 
* **SCL (Serial Clock)**: GPIO15 
* **Botão A**: GPIO5
* **Botão B**: GPIO6 
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

Ao executar o código, espera-se o seguinte comportamento:

1.  **Menu Inicial**:
    * Ao ligar, o display OLED mostrará um menu:
        * "Galton Board"
        * "Press A: Sing" (para modo de bola única)
        * "Press B: Mult" (para modo de múltiplas bolas)
        * "Press A After:"
        * "For BIAS" (para ativar o viés após iniciar a simulação com o Botão A)
    * O programa aguarda a interação do usuário.

2.  **Seleção de Modo**:
    * **Pressionar Botão A (GPIO5)**: Inicia a simulação no modo "bola única".
    * **Pressionar Botão B (GPIO6)**: Inicia a simulação no modo "multi-bolas".

3.  **Ativação do Viés (Bias)**:
    * Se a simulação foi iniciada, um subsequente clique no Botão A ativará o viés (`bias_enabled = true`). O `BIAS` está definido como `0.3`.

4.  **Simulação**:
    * **Pinos**: São desenhados no display.
    * **Bolas**:
        * As bolas são inicializadas no topo central do display.
        * Movem-se para baixo.
        * Ao encontrar uma linha de pinos, a função `random_bin()` decide se a bola se move para a esquerda ou direita.
        * **Modo Bola Única**: Uma bola cai por vez. Após atingir a base, o histograma é mostrado por 2 segundos, e uma nova bola é iniciada.
        * **Modo Multi-Bolas**:
            * Até `12` bolas podem estar ativas simultaneamente.
            * Novas bolas são introduzidas a cada `5` ciclos do loop principal, se houver bolas inativas disponíveis.
            * O histograma é exibido após `62` bolas terem alcançado a base.
    * **Contador de Bolas**: O texto "Total [número]" é exibido no canto superior esquerdo, mostrando o total de bolas lançadas.

5.  **Histograma**:
    * São `16` compartimentos na base do display.
    * Quando uma bola atinge a base, o contador do respectivo compartimento é incrementado.
    * O histograma é desenhado com barras verticais. A altura de cada barra é proporcional ao número de bolas no compartimento, normalizada pela altura máxima `20` pixels e pelo compartimento com mais bolas.
    * **Sem Viés**: Espera-se que o histograma forme uma curva que se aproxima da distribuição normal, com a maioria das bolas concentradas nos compartimentos centrais.
    * **Com Viés (0.3, favorecendo a esquerda)**: Espera-se que o pico do histograma seja deslocado para a esquerda do centro.

6.  **Display**:
    * O display é atualizado continuamente. No modo de simulação (não menu/histograma), há uma pausa de `50 ms` para controlar a velocidade da animação.
    * A resolução do display é 128x64 pixels.
<br>

## 📜 Licença
MIT License - MIT GPL-3.0.

