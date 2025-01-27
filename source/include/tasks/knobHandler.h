#pragma once
#include <Arduino.h>

// Declaration with defaults
void startKnobHandlerTask(
    UBaseType_t core = 1,
    uint32_t stackDepth = 2048,
    UBaseType_t priority = 1);