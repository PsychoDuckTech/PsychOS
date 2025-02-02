#pragma once

#include <Arduino.h>
#include <USBHIDKeyboard.h>
#include "translations/enUS.h"
#include "drivers/multiplexer/CD74HC4067.h"
// #include "../include/usbHIDKeyCodes.h" // by MightyPork on GitHub, customized by GabiBrawl

// Global Constants
extern const char *OS_version;
extern const char *byCompany;
#define PRODUCT_NAME "Kibodo one"

// Matrix Configuration
extern const int totalRows;
extern const int totalCols;
extern const int usesMultiplexer;
extern const int rowPins[];
extern const int colPins[]; // Add this if using non-multiplexer mode
extern Multiplexer colPinsMultiplexer;

// Key Maps & Names
extern const uint8_t keyMapL0[][16];
extern const char *keyNameL0[][16];

// ===== Task Includes (AFTER declarations) =====
#include "../include/tasks.h"

// Task Functions

// Utility Functions
void initializeMatrix();
void submitKeyPress(uint8_t keyCode, bool isPressed);