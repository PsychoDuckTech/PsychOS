#ifndef CLOCK_TASK_H
#define CLOCK_TASK_H

#include <Arduino.h>

extern int hour;
extern int minute;
extern int second;
extern int updated;

void clockTask(void *parameters);

#endif // CLOCK_TASK_H