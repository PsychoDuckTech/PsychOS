#pragma once

#include <Arduino.h>
#include "drivers/multiplexer/CD74HC4067.h"
#include "utils/initializeMatrix.h"

// External Variables from main.cpp
extern Multiplexer colPinsMultiplexer;
extern const int rowPins[];
extern const uint8_t keyMapL0[][16];
extern const char *keyNameL0[][16];
extern const int totalRows;
extern const int totalCols;

void startMatrixScanTask(UBaseType_t core = 0, uint32_t stackDepth = 8192, UBaseType_t priority = 3);