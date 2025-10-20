#pragma once

#include "arduino_compat.h"
#include "usb_hid_compat.h"

#include "translations.h"

#include "drivers/multiplexer/CD74HC4067.h"

// Global Constants
extern const char *OS_version;
extern const char *byCompany;
extern const char *PRODUCT_NAME;

// Pin Definitions
// Matrix pins
extern const int totalRows;
extern const int totalCols;
extern const int rowPins[];

// Multiplexer pins
extern const int MULTIPLEXER_S0;
extern const int MULTIPLEXER_S1;
extern const int MULTIPLEXER_S2;
extern const int MULTIPLEXER_S3;
extern const int MULTIPLEXER_SIG;
extern Multiplexer colPinsMultiplexer;

// Display pins
extern const int LCD_CS;
extern const int LCD_RST;
extern const int LCD_RS;
extern const int SPI_MOSI;
extern const int SPI_SCK;

// Rotary Encoder pins
extern const int KNOB_CLK_PIN;
extern const int KNOB_DT_PIN;
extern const int KNOB_SW_PIN;

// Buzzer pin
extern const int BUZZER_PIN;

// RGB LED pins - defined as macros for compile-time constants
#define RGB_DATA_PIN 3
#define RGB_CLOCK_PIN 46

// Key Maps & Names
extern const uint8_t keyMapL0[][16];
extern const char *keyNameL0[][16];

// ===== Task Includes (AFTER declarations) =====
#include "tasks.h"

// Task Functions

// Utility Functions
void initializeMatrix();
void submitKeyPress(uint8_t keyCode, bool isPressed);