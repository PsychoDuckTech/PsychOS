#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include "display/components/uiComponents.h"
#include "display/displayContent.h"
#include "globals.h"
#include "display/icons.h"

extern Adafruit_ILI9341 tft;

// Button drawing function for consistent UI elements
void drawButton(const char *buttonText, const uint8_t *leftIcon, int leftIconWidth,
                int leftIconHeight, bool showRightArrow, int yPosition, bool selected)
{
    // Constants for button dimensions
    const int UNSELECTED_H = 47;
    const int SELECTED_H = 51;
    const int UNSELECTED_W = 228;
    const int SELECTED_W = 232;

    // Calculate positions
    int itemW = selected ? SELECTED_W : UNSELECTED_W;
    int itemH = selected ? SELECTED_H : UNSELECTED_H;
    int itemY = yPosition - (selected ? 2 : 0);

    // Draw border sprite
    tft.drawBitmap((tft.width() - itemW) / 2, itemY,
                   selected ? SettingsSelectedBorder : SettingsUnselectedBorder,
                   itemW, itemH, TEXT_COLOR);

    // Draw highlight sprite if selected
    if (selected)
    {
        tft.drawBitmap(10, itemY + 6, SettingsSelectedHighlight, 220, 23, HIGHLIGHT_COLOR);
        tft.drawBitmap(10, itemY + 29, SettingsShadow1, 220, 8, 0xCC40);
        tft.drawBitmap(14, itemY + 37, SettingsShadow2, 212, 8, 0x9B20);
    }

    // Draw left icon if provided
    if (leftIcon != nullptr && leftIconWidth > 0 && leftIconHeight > 0)
    {
        int iconX = 16;
        int iconY = itemY + 14;
        tft.drawBitmap(iconX, iconY, leftIcon, leftIconWidth, leftIconHeight,
                       selected ? 0x0 : TEXT_COLOR);
    }

    // Draw text label
    tft.setTextSize(2);
    tft.setFont();
    tft.setTextColor(selected ? 0x0 : TEXT_COLOR);
    tft.setCursor(46, itemY + (selected ? 19 : 17));
    tft.print(buttonText);

    // Draw right arrow icon if requested
    if (showRightArrow)
    {
        int arrowYOffset = selected ? 16 : 14;
        tft.drawBitmap(214, itemY + arrowYOffset, iconArrowRight, 12, 20,
                       selected ? 0x0 : TEXT_COLOR);
    }
}

// Screen title rendering function
void drawScreenTitle(const char *title, int yPos, int textSize, uint16_t textColor, const GFXfont *font)
{
    tft.setTextSize(textSize);
    if (font)
    {
        tft.setFont(font);
    }
    else
    {
        tft.setFont();
    }
    tft.setTextColor(textColor);

    // Calculate title width (multiplier 18 for default font size 3, adjust for custom fonts)
    int charWidth = (textSize == 3 && font == nullptr) ? 18 : 16;
    int titleWidth = strlen(title) * charWidth;

    // Center the title
    tft.setCursor((tft.width() - titleWidth) / 2, yPos);
    tft.print(title);
}

// Slider button component
void drawSliderButton(const char *buttonText, int value, int maxValue, const uint8_t *leftIcon,
                     int leftIconWidth, int leftIconHeight, bool showPercentage,
                     int yPosition, bool selected)
{
    // Constants for button dimensions
    const int UNSELECTED_H = 47;
    const int SELECTED_H = 51;
    const int UNSELECTED_W = 228;
    const int SELECTED_W = 232;
    
    // Calculate positions
    int itemW = selected ? SELECTED_W : UNSELECTED_W;
    int itemH = selected ? SELECTED_H : UNSELECTED_H;
    int itemY = yPosition - (selected ? 2 : 0);
    
    // Draw border sprite
    tft.drawBitmap((tft.width() - itemW) / 2, itemY,
                  selected ? SettingsSelectedBorder : SettingsUnselectedBorder,
                  itemW, itemH, TEXT_COLOR);
    
    // Draw highlight sprite if selected
    if (selected) {
        tft.drawBitmap(10, itemY + 6, SettingsSelectedHighlight, 220, 23, HIGHLIGHT_COLOR);
        tft.drawBitmap(10, itemY + 29, SettingsShadow1, 220, 8, 0xCC40);
        tft.drawBitmap(14, itemY + 37, SettingsShadow2, 212, 8, 0x9B20);
    }
    
    // Draw left icon if provided
    if (leftIcon != nullptr && leftIconWidth > 0 && leftIconHeight > 0) {
        int iconX = 16;
        int iconY = itemY + 14;
        tft.drawBitmap(iconX, iconY, leftIcon, leftIconWidth, leftIconHeight,
                      selected ? 0x0 : TEXT_COLOR);
    }
    
    // Draw text label
    tft.setTextSize(2);
    tft.setFont();
    tft.setTextColor(selected ? 0x0 : TEXT_COLOR);
    tft.setCursor(46, itemY + (selected ? 12 : 10));
    tft.print(buttonText);
    
    // Draw slider bar
    const int SLIDER_X = 46;
    const int SLIDER_Y = itemY + (selected ? 32 : 30);
    const int SLIDER_W = 140;
    const int SLIDER_H = 8;
    
    // Calculate fill width based on value
    int fillWidth = map(value, 0, maxValue, 0, SLIDER_W);
    
    // Draw slider background
    tft.fillRoundRect(SLIDER_X, SLIDER_Y, SLIDER_W, SLIDER_H, 4, selected ? 0x0 : ULTRA_MUTED_COLOR);
    // Draw slider filled portion
    tft.fillRoundRect(SLIDER_X, SLIDER_Y, fillWidth, SLIDER_H, 4, selected ? 0x0 : SELECTED_COLOR);
    
    // Draw value text
    tft.setTextSize(1);
    tft.setCursor(SLIDER_X + SLIDER_W + 10, SLIDER_Y + 6);
    tft.print(value);
    if (showPercentage) {
        tft.print("%");
    }
}