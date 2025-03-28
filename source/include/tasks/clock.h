#pragma once

#include <Arduino.h>
#include "main.h"
#include "globals.h"

// Declare variables as extern (no initialization)
extern int hours;
extern int minutes;
extern int seconds;
extern bool updatedMinutes;

// Function declarations with default parameters
void clockTask(void *parameters);
void updateClock(int newHours = -1, int newMinutes = -1, int newSeconds = -1);
void startClockTask(UBaseType_t core = 0, uint32_t stackDepth = 1024, UBaseType_t priority = 1);