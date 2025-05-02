# 🧪 Teste Unitário para a Função de Conversão de Temperatura no Raspberry Pi Pico W
<br>

## 🎯 Objetivo do Projeto

Este projeto tem como objetivo implementar, em linguagem C, uma função que converte valores digitais lidos do ADC interno da Raspberry Pi Pico W em temperatura em graus Celsius e, a partir disso, implementa um **teste unitário utilizando a biblioteca Unity** para verificar a precisão da função de conversão com um valor simulado..

O projeto visa demonstrar:

- Conversão precisa da leitura para temperatura.
- Teste unitário com valor conhecido (0.876V → 27 °C) com a biblioteca Unity.
<br>

## 📂 Estrutura do Projeto
```text
unit_test/
├── app/
│   └── adc_to_celsius.c             # Implementação da Função de Conversão        
├── include/
│   └── adc_to_celsius.h             # Declaração da Função de Conversão    
├── test/
│   ├── Unity/                       # Biblioteca Unity (arquivos)
│   │    └── unity_internas.h      
│   │    └── unity.c  
│   │    └── unity.h       
│   ├── test_adc_to_celsius.c        # Teste Unitário
│   └── test_adc_to_celsius.exe      # Executável do Teste Unitário
├── .gitignore                       # Ignora determinados Arquivos
├── CMakeLists.txt                   # Configuração de Build
├── pico_sdk_import.cmake            # Configuração Automática do Ambiente de Compilação
└── README.md                        # README
```
<br>

## 🧩 Componentes Necessários

- GCC Compiler
- Pico SDK
- Biblioteca Unity
<br>

## 🧮 Fórmula de Conversão

$$
T(°C) = 27 - \frac{(ADC_{reading} \times 3.3 / 4095) - 0.706}{0.001721}
$$
<br>

## 🛠️ Como compilar e executar
Para compilar e executar o código, no diretório raiz, executar os seguintes comandos no terminal:
```text
gcc tests/test_adc_to_celsius.c tests/Unity/unity.c app/adc_to_celsius.c -Iinclude -Isrc -o tests/test_adc_to_celsius.exe

./tests/test_adc_to_celsius.exe
```

## 📈 Resultados esperados ou observados
É esperado que apareça a seguinte mensagem no terminal:
```text
test1_adc: PASS
-----------------------
1 Tests 0 Failures 0 Ignored
```

