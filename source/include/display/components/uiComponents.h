#pragma once

#include <Arduino.h>
#include "../../globals.h"

// UI Component function declarations

// Button drawing function for consistent UI elements
void drawButton(const char *buttonText, const uint8_t *leftIcon = nullptr, int leftIconWidth = 0,
                int leftIconHeight = 0, bool showRightArrow = true, int yPosition = 0, bool selected = false);

// Screen title rendering function
void drawScreenTitle(const char *title, int yPos = 24, int textSize = 3, uint16_t textColor = 0xFFFF, const GFXfont *font = nullptr);