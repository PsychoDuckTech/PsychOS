#pragma once
#include "arduino_compat.h"

void printKeyPollingRates(int totalRows, int totalCols, unsigned long **pollCount, unsigned long &lastTime);