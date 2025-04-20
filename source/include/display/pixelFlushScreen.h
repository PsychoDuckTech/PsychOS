#pragma once

#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include "../globals.h"

// Function declarations for pixel flush screen
void displayPixelFlushScreen(void *parameters);
void startPixelFlush(void *parameters);
void startFlushProcess();
void drawDurationSelector();

// Knob event handlers for pixel flush screen
void handlePixelFlushKnobRotation(int direction);
void handlePixelFlushKnobPress();
void handlePixelFlushLongPress();

// External variables
extern volatile bool pixelFlushRunning;
extern volatile bool pixelFlushCancelled;
extern int selectedDurationIndex;
extern const int durationOptions[];
extern const int numDurationOptions;
