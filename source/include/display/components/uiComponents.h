#pragma once

#include <Arduino.h>
#include "../../globals.h"

// UI Component function declarations

// Button drawing function for consistent UI elements
void drawButton(const char *buttonText, const uint8_t *leftIcon = nullptr, int leftIconWidth = 0,
                int leftIconHeight = 0, bool showRightArrow = true, int yPosition = 0, bool selected = false);

// Screen title rendering function
void drawScreenTitle(const char *title, int yPos = 24, int textSize = 3, uint16_t textColor = TEXT_COLOR, const GFXfont *font = nullptr);

// New slider button component
void drawSliderButton(const char *buttonText, int value, int maxValue, const uint8_t *leftIcon = nullptr,
                      int leftIconWidth = 0, int leftIconHeight = 0, bool showPercentage = false,
                      int yPosition = 0, bool selected = false);

// Text value button component (for options with text values instead of numeric sliders)
void drawSliderButtonWithText(const char *buttonText, const char *valueText, const uint8_t *leftIcon = nullptr,
                              int leftIconWidth = 0, int leftIconHeight = 0,
                              int yPosition = 0, bool selected = false);