#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include "display/components/uiComponents.h"
#include "display/displayContent.h"
#include "globals.h"
#include "display/icons.h"
#include "main.h"
#include "translations.h"

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

    // Draw border
    int frameX = (tft.width() - itemW) / 2;
    if (selected)
    {
        // Draw selected border using frame with thick lines (4px)
        drawFrame(frameX, itemY, itemW, itemH, TEXT_COLOR, 0);
    }
    else
    {
        // Draw unselected border using frame with thin lines (2px)
        drawFrame(frameX, itemY, itemW, itemH, TEXT_COLOR, 1);
    }

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

    // Draw border
    int frameX = (tft.width() - itemW) / 2;
    if (selected)
    {
        // Draw selected border using frame with thick lines (4px)
        drawFrame(frameX, itemY, itemW, itemH, TEXT_COLOR, 0);
    }
    else
    {
        // Draw unselected border using frame with thin lines (2px)
        drawFrame(frameX, itemY, itemW, itemH, TEXT_COLOR, 1);
    }

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
    const int SLIDER_H = 8;   // Height of the background slider

    // Define smaller filled part dimensions
    const int FILL_H = 4;                                  // Smaller height for the filled part
    const int BORDER_PADDING = 2;                          // Padding on left and right sides of filled portion
    const int FILL_Y = SLIDER_Y + (SLIDER_H - FILL_H) / 2; // Centered vertically in background

    // Calculate fill width based on value, ensuring it fits within the slider with borders
    int maxFillWidth = SLIDER_W - (BORDER_PADDING * 2);
    int fillWidth = map(value, 0, maxValue, 0, maxFillWidth);

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

    // Draw slider filled portion (smaller and centered vertically) with padding on both sides
    if (fillWidth > 0)
    {
        tft.fillRoundRect(SLIDER_X + BORDER_PADDING, FILL_Y, fillWidth, FILL_H, 2, sliderFillColor);
    }

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
    int itemX = (tft.width() - itemW) / 2; // Center the button horizontally

    // Draw border with appropriate thickness (4px when selected, 2px when not)
    drawFrame(itemX, itemY, itemW, itemH, TEXT_COLOR, selected ? 0 : 1);

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
                   selected ? 0x0 : TEXT_COLOR);

    // Draw right arrow icon
    int rightArrowX = 200;
    tft.drawBitmap(rightArrowX, arrowY, iconArrowRight, 12, 20,
                   selected ? 0x0 : TEXT_COLOR);

    // Draw the value text centered between arrows
    tft.setTextSize(2);
    tft.setFont();

    // Calculate text width more accurately
    int charWidth = 11; // More precise width per character at text size 2
    int textWidth = strlen(valueText) * charWidth;

    // Calculate the available space between arrows
    int arrowSpacing = rightArrowX - leftArrowX - 12; // 12 is the width of left arrow

    // Center the value text exactly between the arrows (horizontally)
    int centerX = leftArrowX + 12 + (arrowSpacing / 2);
    int valueX = centerX - (textWidth / 2);

    // Position text higher in the button (moved up from 24px to 19px from top)
    int valueY = itemY + 19;

    // Make value text stand out
    uint16_t valueColor = selected ? 0x0 : TEXT_COLOR;

    // Draw the value text
    tft.setTextColor(valueColor);
    tft.setCursor(valueX, valueY);
    tft.print(valueText);
}

// Footer drawing function for consistent footer across screens
void drawFooter()
{
    tft.setTextSize(1);
    tft.setFont();

    // Calculate width of each text line for proper centering
    int displayWidth = tft.width();
    int charWidth = 6; // Approximate width per character at text size 1

    // Center "Secured by Dux"
    int securedByWidth = strlen(ui_secured_by) * charWidth;
    int securedByX = (displayWidth - securedByWidth) / 2;
    tft.setTextColor(ULTRA_MUTED_COLOR);
    tft.setCursor(securedByX, 287);
    tft.print(ui_secured_by);

    // Center "Powered by PsychOS"
    int poweredByWidth = strlen(ui_powered_by) * charWidth;
    int poweredByX = (displayWidth - poweredByWidth) / 2;
    tft.setTextColor(MUTED_COLOR);
    tft.setCursor(poweredByX, 296);
    tft.print(ui_powered_by);

    // Center "Build X.X.X" with version
    char versionStr[30];
    snprintf(versionStr, sizeof(versionStr), "%s %s", ui_build, OS_version);
    int versionWidth = strlen(versionStr) * charWidth;
    int versionX = (displayWidth - versionWidth) / 2;
    tft.setCursor(versionX, 305);
    tft.print(ui_build);
    tft.print(" ");
    tft.print(OS_version);
}

void drawFrame(int x, int y, int width, int height, uint16_t color, int thickness)
{
    int cornerSize = thickness == 1 ? 13 : 15; // Size of the corner bitmap

    // Select corner bitmaps based on thickness
    const unsigned char *tlCorner = thickness == 1 ? cornerTLUnselected : cornerTL;
    const unsigned char *trCorner = thickness == 1 ? cornerTRUnselected : cornerTR;
    const unsigned char *blCorner = thickness == 1 ? cornerBLUnselected : cornerBL;
    const unsigned char *brCorner = thickness == 1 ? cornerBRUnselected : cornerBR;

    // Draw the four corners
    tft.drawBitmap(x, y, tlCorner, cornerSize, cornerSize, color);                                            // Top-left
    tft.drawBitmap(x + width - cornerSize, y, trCorner, cornerSize, cornerSize, color);                       // Top-right
    tft.drawBitmap(x, y + height - cornerSize, blCorner, cornerSize, cornerSize, color);                      // Bottom-left
    tft.drawBitmap(x + width - cornerSize, y + height - cornerSize, brCorner, cornerSize, cornerSize, color); // Bottom-right

    if (thickness == 0)
    {
        // Draw double-pixel lines
        tft.drawFastHLine(x + cornerSize, y, width - 2 * cornerSize, color);              // Top line 1
        tft.drawFastHLine(x + cornerSize, y + 1, width - 2 * cornerSize, color);          // Top line 2
        tft.drawFastHLine(x + cornerSize, y + height - 2, width - 2 * cornerSize, color); // Bottom line 1
        tft.drawFastHLine(x + cornerSize, y + height - 1, width - 2 * cornerSize, color); // Bottom line 2

        tft.drawFastVLine(x, y + cornerSize, height - 2 * cornerSize, color);             // Left line 1
        tft.drawFastVLine(x + 1, y + cornerSize, height - 2 * cornerSize, color);         // Left line 2
        tft.drawFastVLine(x + width - 2, y + cornerSize, height - 2 * cornerSize, color); // Right line 1
        tft.drawFastVLine(x + width - 1, y + cornerSize, height - 2 * cornerSize, color); // Right line 2
    }
    else
    {
        // Draw four-pixel lines using fillRect for better performance
        // Top border
        tft.fillRect(x + cornerSize, y, width - 2 * cornerSize, 4, color);
        // Bottom border
        tft.fillRect(x + cornerSize, y + height - 4, width - 2 * cornerSize, 4, color);
        // Left border
        tft.fillRect(x, y + cornerSize, 4, height - 2 * cornerSize, color);
        // Right border
        tft.fillRect(x + width - 4, y + cornerSize, 4, height - 2 * cornerSize, color);
    }
}

// Help indicators drawing function for consistent help texts across screens
void drawHelpIndicators(int yStartPosition, uint16_t textColor, bool showRotate, bool showPress, bool showLongPress)
{
    tft.setTextSize(1);
    tft.setFont();
    tft.setTextColor(textColor);

    int lineHeight = 8; // Space between lines
    int currentY = yStartPosition;
    const int leftMargin = 15; // Consistent left margin for all help text

    if (showRotate)
    {
        tft.setCursor(leftMargin, currentY);
        tft.print(ui_rotate_adjust);
        currentY += lineHeight;
    }

    if (showPress)
    {
        tft.setCursor(leftMargin, currentY);
        tft.print(ui_press_next);
        currentY += lineHeight;
    }

    if (showLongPress)
    {
        tft.setCursor(leftMargin, currentY);
        tft.print(ui_long_press_quit);
    }
}