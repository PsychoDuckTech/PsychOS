#ifndef GLOBALS_H
#define GLOBALS_H

#include <Adafruit_ILI9341.h>

extern Adafruit_ILI9341 tft;
extern bool connectionStatus;
extern bool moduleConnectionStatus;
extern bool capsLockStatus;
extern int hours;
extern int minutes;
extern int updatedMinutes;

extern int settingsSelectedOption;
extern bool inSettingsSubmenu;

#endif // GLOBALS_H