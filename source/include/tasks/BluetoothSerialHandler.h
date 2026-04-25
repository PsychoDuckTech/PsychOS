#pragma once

#include <Arduino.h>

void bluetoothSerialHandler(void *parameters);
void startBluetoothSerialHandlerTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority);