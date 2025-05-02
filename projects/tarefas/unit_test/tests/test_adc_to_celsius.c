// Bibliotecas e Arquivos incluídos para o Projeto
#include "Unity/unity.h"
#include "../include/adc_to_celsius.h"

// Configuração (setUp) e limpeza (tearDown) 
void setUp(void) {}
void tearDown(void) {}

// Caso de teste: Verifica conversão ADC para Celsius
void test1_adc(void){
    // Testa se o valor convertido está dentro de 1 grau de diferença do esperado (27°C) para uma entrada ADC de 876
    TEST_ASSERT_FLOAT_WITHIN(1, 27, adc_to_celsius(876));
}

// Função principal
int main(void) {
    // Inicializa o framework de testes
    UNITY_BEGIN();
    
    // Registra e executa o teste
    RUN_TEST(test1_adc);
    
    // Finaliza os testes e retorna o status
    return UNITY_END();
}