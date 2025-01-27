#ifndef CLOCK_TASK_H
#define CLOCK_TASK_H

#include <Arduino.h>

// Declare variables as extern (no initialization)
extern int hours;
extern int minutes;
extern int seconds;
extern bool updatedMinutes;

// Function declarations with default parameters
void clockTask(void *parameters);
void updateClock(int newHours = -1, int newMinutes = -1, int newSeconds = -1);
void startClockTask(UBaseType_t core = 1, uint32_t stackDepth = 1024, UBaseType_t priority = 1);

#endif // CLOCK_TASK_H