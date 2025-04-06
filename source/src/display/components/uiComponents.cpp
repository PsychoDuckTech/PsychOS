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
    tft.setCursor(46, itemY + (selected ? 12 : 10));
    tft.print(buttonText);

    // Draw slider bar
    const int SLIDER_X = 46;
    const int SLIDER_Y = itemY + (selected ? 32 : 30);
    const int SLIDER_W = 110; // Reduced width to make room for value
    const int SLIDER_H = 8;

    // Calculate fill width based on value
    int fillWidth = map(value, 0, maxValue, 0, SLIDER_W);

    // Define different colors for filled and unfilled parts
    uint16_t sliderBgColor, sliderFillColor;

    if (selected)
    {
        // When selected: gray background with dark gray fill
        sliderBgColor = ULTRA_MUTED_COLOR; // Very light gray for background
        sliderFillColor = MUTED_COLOR;     // Darker gray for the filled part
    }
    else
    {
        // When unselected: muted background with highlight fill
        sliderBgColor = MUTED_COLOR;       // Gray background
        sliderFillColor = HIGHLIGHT_COLOR; // Yellowish fill
    }

    // Draw slider background (unfilled part)
    tft.fillRoundRect(SLIDER_X, SLIDER_Y, SLIDER_W, SLIDER_H, 4, sliderBgColor);
    // Draw slider filled portion
    tft.fillRoundRect(SLIDER_X, SLIDER_Y, fillWidth, SLIDER_H, 4, sliderFillColor);

    // Draw value text - larger and properly positioned
    tft.setTextSize(2);                            // Same size as button text
    tft.setTextColor(selected ? 0x0 : TEXT_COLOR); // Same color as button text

    // Convert value to string to calculate its width
    char valueStr[10];
    if (showPercentage)
    {
        snprintf(valueStr, sizeof(valueStr), "%d%%", value);
    }
    else
    {
        snprintf(valueStr, sizeof(valueStr), "%d", value);
    }

    // Calculate text width for positioning
    int charWidth = 12; // Approximate width per character at text size 2
    int textWidth = strlen(valueStr) * charWidth;

    // Position value on the far right side but still inside the button
    // The button width is approximately 220px, leave proper margin
    int valueX = 215 - textWidth;

    // Center the value vertically in the button
    int valueY = itemY + (selected ? 19 : 17); // Same vertical alignment as main text

    // Draw the value
    tft.setCursor(valueX, valueY);
    tft.print(value);
    if (showPercentage)
    {
        tft.print("%");
    }
}

// Text value button component with simplified layout (just left/right arrows and centered text)
void drawSliderButtonWithText(const char *buttonText, const char *valueText, const uint8_t *leftIcon,
                              int leftIconWidth, int leftIconHeight,
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
    if (selected)
    {
        tft.drawBitmap(10, itemY + 6, SettingsSelectedHighlight, 220, 23, HIGHLIGHT_COLOR);
        tft.drawBitmap(10, itemY + 29, SettingsShadow1, 220, 8, 0xCC40);
        tft.drawBitmap(14, itemY + 37, SettingsShadow2, 212, 8, 0x9B20);
    }

    // Draw left arrow icon
    int leftArrowX = 30;
    int arrowY = itemY + (itemH / 2) - 6; // Center arrows vertically
    tft.drawBitmap(leftArrowX, arrowY, iconArrowLeft, 12, 20,
                   selected ? 0x0 : MUTED_COLOR);

    // Draw right arrow icon
    int rightArrowX = 218;
    tft.drawBitmap(rightArrowX, arrowY, iconArrowRight, 12, 20,
                   selected ? 0x0 : MUTED_COLOR);

    // Draw the value text centered between arrows
    tft.setTextSize(2);
    tft.setFont();

    // Calculate text width for positioning (approximated)
    int charWidth = 12; // Approximate width per character at text size 2
    int textWidth = strlen(valueText) * charWidth;

    // Center the value text in the button
    int centerX = 120;                      // Center of screen (240/2)
    int valueX = centerX - (textWidth / 2); // Center the text
    int valueY = itemY + (itemH / 2) + 6;   // Center text vertically

    // Make value text stand out
    uint16_t valueColor = selected ? 0x0 : HIGHLIGHT_COLOR;

    // Draw the value text
    tft.setTextColor(valueColor);
    tft.setCursor(valueX, valueY);
    tft.print(valueText);
}