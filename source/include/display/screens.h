#pragma once

#include <Adafruit_ILI9341.h>
#include "arduino_compat.h"
#include "displayContent.h"
#include "globals.h"
#include "icons.h"
#include "main.h"
#include "translations.h"
#include "components/uiComponents.h"

// External references to globals
extern Adafruit_ILI9341 tft;
extern bool connectionStatus;
extern bool moduleConnectionStatus;
extern bool capsLockStatus;
extern int hours;
extern int minutes;
extern bool needsFullRedraw;
extern int rgbValues[4];

// Main screen functions
void displayMainScreen(void *parameters);
void displayTopBar(void *parameters);
void displayTime(void *parameters);
void displayWPM(void *parameters);
void displayLAYER(void *parameters);
void displayDemo(void *parameters);

// Settings screen functions
void drawSettingsStatic(void *parameters);
void displaySettingsScreen(void *parameters);

// RGB submenu screen functions
void displayRGBSubmenu(void *parameters);

// Clock submenu screen functions
void displayClockSubmenu(void *parameters);

void displayPixelFlushScreen(void *parameters);