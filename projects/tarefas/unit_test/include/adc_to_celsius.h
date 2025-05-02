// Constantes do Sistema
#define ADC_RESOLUTION 4095 // Resolução do ADC 
#define V_REF 3.3 // Tensão de Referência (Volts)

// Declaração da função para receber o valor digital do sensor interno e converter para Celsius
float adc_to_celsius(uint16_t adc_val);