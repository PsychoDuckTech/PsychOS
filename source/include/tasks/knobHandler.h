#pragma once
#include <Arduino.h>

// Adicionar a declaração para a função ISR
void IRAM_ATTR knobButtonISR();

// Adicionar uma fila (Queue) para eventos de botão para desacoplar a ISR da tarefa
extern QueueHandle_t knobButtonQueue;

// Declaração com defaults
void startKnobHandlerTask(UBaseType_t core = 1, uint32_t stackDepth = 2048, UBaseType_t priority = 1);
