#ifndef RTOS_RESOURCES
#define RTOS_RESOURCES

#include "FreeRTOS.h" // Biblioteca FreeRTOS
#include "semphr.h" // Biblioteca de semáforos do FreeRTOS
#include "task.h" // Biblioteca de tarefas do FreeRTOS

// Estrutura de estado da máquina de estados
typedef enum state_t { IDLE, MIC, TEMP } state_t;

// Variáveis globais para o estado do sistema e semáforos
extern state_t state;
extern SemaphoreHandle_t rMutex;
extern TaskHandle_t mic_handle;
extern TaskHandle_t temp_handle;

#endif // !RTOS_RESOURCES
