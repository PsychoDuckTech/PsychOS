#pragma once

#include <Arduino.h>
#include "tasks/clock.h"

void serialHandler(void *parameters);
void startSerialTask(UBaseType_t core = 1, uint32_t stackDepth = 4096, UBaseType_t priority = 1);