# 💡 Piscar o LED Embutido da Raspberry Pi Pico W com Arquitetura Modular
<br>

## 🎯 Objetivo do projeto

Este projeto tem como objetivo reorganizar de forma modular o código que faz o LED embutido da **Raspberry Pi Pico W** piscar indefinidamente. A estrutura modular separa a lógica da aplicação, a abstração de hardware e os drivers, seguindo boas práticas de desenvolvimento embarcado.
<br>

## 💡 Camadas de Software

| Camada        | Responsabilidade | 
|--------------------|-----------|
| Aplicação |  Lógica de negócios        |  
| HAL   | Abstração do Hardware    | 
| Drivers   | Controle direto do Hardware    |
<br>

## 📁 Estrutura Modular do Projeto

```text
projeto/
├── app/
│   └── main.c                  # Arquivo da camada de aplicação
├── drivers/
│   └── led_embutido.c          # Driver de controle direto do LED com cyw43_arch
├── hal/
│   └── hal_led.c               # Camada de abstração para o controle de LED
├── include/
│   ├── led_embutido.h          # Cabeçalho do driver
│   └── hal_led.h               # Cabeçalho da HAL
└── CMakeLists.txt              # Script para o build do projeto
```
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
É esperado que o LED pisque indefinidamente