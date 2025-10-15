#pragma once

#include <Arduino.h>
#include "../../globals.h"

// Menu item structure for scrollable menus
struct ScrollableMenuItem {
    const char *text;
    const uint8_t *icon;
    int iconWidth;
    int iconHeight;
    void (*action)();  // Function to call when selected
};

// Settings menu configuration - centralized
extern const ScrollableMenuItem settingsMenuItems[];
extern const int SETTINGS_MENU_ITEM_COUNT;

// UI Component function declarations

// Button drawing function for consistent UI elements
void drawButton(const char *buttonText, const uint8_t *leftIcon = nullptr, int leftIconWidth = 0,
                int leftIconHeight = 0, bool showRightArrow = true, int yPosition = 0, bool selected = false);

// Screen title rendering function
void drawScreenTitle(const char *title, int yPos = 12, int textSize = 3, uint16_t textColor = TEXT_COLOR, const GFXfont *font = nullptr);

// New slider button component
void drawSliderButton(const char *buttonText, int value, int maxValue, const uint8_t *leftIcon = nullptr,
                      int leftIconWidth = 0, int leftIconHeight = 0, bool showPercentage = false,
                      int yPosition = 0, bool selected = false);

// Text value button component (for options with text values instead of numeric sliders)
void drawSliderButtonWithText(const char *buttonText, const char *valueText, const uint8_t *leftIcon = nullptr,
                              int leftIconWidth = 0, int leftIconHeight = 0,
                              int yPosition = 0, bool selected = false);

// Footer drawing function for consistent footer across screens
void drawFooter();

// Help indicators drawing function for consistent help texts across screens
void drawHelpIndicators(int yStartPosition = 250, uint16_t textColor = MUTED_COLOR, bool showRotate = true, bool showPress = true, bool showLongPress = true);

void drawFrame(int x, int y, int width, int height, uint16_t color, int thickness = 1);