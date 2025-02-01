#ifndef DISPLAY_CONTENT_H
#define DISPLAY_CONTENT_H

#include "tasks/wpmCounter.h"

void displayMainScreen(void *parameters);
void displayTopBar(void *parameters);
void displayTime(void *parameters);
void clearTime(void *parameters);
void clearWPM(void *parameters);
void displayWPM(void *parameters);
void displayDemo(void *parameters);
void displaySettingsScreen(void *parameters);
void displayRGBSubmenu(void *parameters);

#endif // DISPLAY_CONTENT_H