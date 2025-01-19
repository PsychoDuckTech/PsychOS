#ifndef CLOCK_TASK_H
#define CLOCK_TASK_H

#include <Arduino.h>

extern int hour;
extern int minute;
extern int second;
extern int updated;

int hours = 0;
int minutes = 0;
int seconds = 0;
int updatedMinutes = true;

void clockTask(void *parameters);
void updateClock(int newHours = -1, int newMinutes = -1, int newSeconds = -1);

#endif // CLOCK_TASK_H