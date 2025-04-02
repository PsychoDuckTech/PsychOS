#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include "displayContent.h"
#include "globals.h"
#include "icons.h"
#include "main.h"
#include "translations.h"

extern Adafruit_ILI9341 tft;
extern bool connectionStatus;
extern bool moduleConnectionStatus;
extern bool capsLockStatus;
extern int hours;
extern int minutes;
bool needsFullRedraw = true;

int rgbValues[4] = {255, 255, 255, 100};

static unsigned long lastToggleTime = 0;
static bool toggleDiscIcon = false;

// Button drawing function for consistent UI elements
void drawButton(const char *buttonText, const uint8_t *leftIcon = nullptr, int leftIconWidth = 0,
                int leftIconHeight = 0, bool showRightArrow = true, int yPosition = 0, bool selected = false)
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

// Universal title rendering function
void renderScreenTitle(const char *title, int yPos = 24, int textSize = 3, uint16_t textColor = TEXT_COLOR, const GFXfont *font = nullptr)
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

void displayMainScreen(void *parameters)
{
    needsFullRedraw = true;

    displayTopBar(parameters);
    displayTime(parameters);
    displayWPM(parameters);
    displayLAYER(parameters);
    displayDemo(parameters); // demo is the last thing being loaded as it takes the longest at loading.
}

void displayTopBar(void *parameters)
{
    // Check if 750ms have passed to toggle the icon
    if (connectionStatus && millis() - lastToggleTime >= 750)
    {
        toggleDiscIcon = !toggleDiscIcon;
        lastToggleTime = millis();
    }

    tft.drawBitmap(11, 9, connectionStatus ? (toggleDiscIcon ? iconDisc1 : iconDisc2) : iconDiscMuted, 16, 15, connectionStatus ? SUCCESS_COLOR : ERROR_COLOR, BG_COLOR);
    tft.drawBitmap(moduleConnectionStatus ? 35 : 36, 9, moduleConnectionStatus ? iconBleConnected : iconBleDisconnected, 14, 15, moduleConnectionStatus ? SUCCESS_COLOR : MUTED_COLOR, BG_COLOR);

    tft.setTextSize(1);
    tft.setTextColor(TEXT_COLOR);
    tft.setFont();
    tft.setCursor(206, 8);
    tft.print(ui_caps);

    // Add a static variable to track the previous capsLockStatus
    static bool previousCapsLockStatus = !capsLockStatus; // Initialize to a different value to ensure the first update

    // Check if capsLockStatus has changed
    if (capsLockStatus != previousCapsLockStatus)
    {
        tft.drawBitmap(206, 18, capsLockStatus ? textOff : textOn, capsLockStatus ? 14 : 9, 5, capsLockStatus ? BG_COLOR : BG_COLOR, BG_COLOR);
        previousCapsLockStatus = capsLockStatus; // Update the previous status
    }

    tft.drawBitmap(206, 18, capsLockStatus ? textOn : textOff, capsLockStatus ? 9 : 14, 5, capsLockStatus ? HIGHLIGHT_COLOR : MUTED_COLOR, BG_COLOR);
}

void displayTime(void *parameters)
{
    clearTime(parameters);
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(8);
    tft.setFont();

    char timeString[3];
    sprintf(timeString, "%02d", hours);
    tft.setCursor(75, 60);
    tft.print(timeString);

    sprintf(timeString, "%02d", minutes);
    tft.setCursor(75, 122);
    tft.print(timeString);
}

void clearTime(void *parameters)
{
    tft.fillRect(75, 51, 90, 130, BG_COLOR);
}

void displayWPM(void *parameters)
{
    clearWPM(parameters);
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(2);
    tft.setFont();
    tft.setCursor(11, 297);
    tft.print(String(WPMCounter::wpm));

    // Adjust the position of "WPM" based on the number of digits in WPM
    int wpmLength = String(WPMCounter::wpm).length();
    int wpmOffset = 24 + (wpmLength - 1) * 12; // Adjust offset dynamically

    tft.setTextColor(MUTED_COLOR);
    tft.setTextSize(1);
    tft.setCursor(wpmOffset, 304);
    tft.print(ui_wpm);
}

void displayLAYER(void *parameters)
{
    tft.setTextColor(MUTED_COLOR);
    tft.setTextSize(1);
    tft.setFont();
    tft.setCursor(186, 304);
    tft.print(ui_layer);
    tft.setTextColor(HIGHLIGHT_COLOR);
    tft.setTextSize(2);
    tft.setCursor(219, 297);
    tft.print("3");
}

void displayDemo(void *parameters)
{
    tft.drawBitmap(9, 221, MusicPlayerBorder, 222, 58, TEXT_COLOR);
    tft.drawBitmap(13, 262, MusicPlayerShadow1, 214, 17, 0x18C3);
    tft.drawBitmap(13, 252, MusicPlayerShadow2, 214, 11, 0x861);

    tft.setTextSize(2);
    tft.setTextColor(TEXT_COLOR);
    tft.setFont();
    tft.setCursor(21, 233);
    tft.print(ui_now_playing);

    tft.setCursor(21, 253);
    tft.print(ui_song_title);

    tft.drawBitmap(200, 241, iconMusicPlay, 20, 18, TEXT_COLOR);
}

void drawSettingsStatic(void *parameters)
{
    tft.fillScreen(BG_COLOR);
    // Draw title
    renderScreenTitle(ui_settings);

    // Draw footer text before options are loaded
    tft.setTextSize(1);
    tft.setFont();
    tft.setTextColor(ULTRA_MUTED_COLOR);
    tft.setCursor(76, 287);
    tft.print(ui_secured_by);
    tft.setTextColor(MUTED_COLOR);
    tft.setCursor(65, 296);
    tft.print(ui_powered_by);
    tft.setCursor(79, 305);
    tft.print(ui_build);
    tft.print(" ");
    tft.print(OS_version);
}

void displaySettingsScreen(void *parameters)
{
    if (needsFullRedraw)
    {
        drawSettingsStatic(parameters);
        needsFullRedraw = false;
    }

    tft.fillRect(4, 60, 232, 215, BG_COLOR);

    // Menu configuration
    const int MENU_START_Y = 62;
    const int ITEM_SPACING = 54;
    const char *menuItems[] = {ui_modules, ui_underglow, ui_clock, ui_iot_link};

    // Icons for each menu item
    const uint8_t *icons[] = {iconBleConnectedBig, iconLightBulb, iconTranslation, iconIotChip};
    const int iconWidths[] = {14, 18, 22, 22};
    const int iconHeights[] = {22, 23, 22, 22};

    // Draw menu items with our new button function
    for (int i = 0; i < 4; i++)
    {
        bool selected = (i == settingsSelectedOption);
        int baseY = MENU_START_Y + (i * ITEM_SPACING);

        // Use the new drawButton function with appropriate icons
        drawButton(menuItems[i], icons[i], iconWidths[i], iconHeights[i], true, baseY, selected);
    }
}

void displayRGBSubmenu(void *parameters)
{
    const char *rgbOptions[] = {ui_red, ui_green, ui_blue, ui_brightness};
    const int valueRanges[] = {255, 255, 255, 100};
    needsFullRedraw = true;

    if (rgbState.needsRefresh)
    {
        tft.fillScreen(BG_COLOR);

        // Title using universal function
        renderScreenTitle(ui_underglow, 30);

        // Draw all elements
        for (int i = 0; i < 4; i++)
        {
            const int yPos = 70 + (i * 50);

            // Option label
            tft.setTextSize(1);
            tft.setFont(&FreeSansBold9pt7b);
            tft.setTextColor((i == rgbState.currentSelection) ? HIGHLIGHT_COLOR : TEXT_COLOR);
            tft.setCursor(20, yPos - 5);
            tft.print(rgbOptions[i]);

            // Value bar
            const float fillWidth = map(rgbState.values[i], 0, valueRanges[i], 0, 160);
            tft.fillRoundRect(20, yPos, 160, 20, 5, BG_COLOR);
            tft.fillRoundRect(20, yPos, fillWidth, 20, 5, (i == rgbState.currentSelection) ? HIGHLIGHT_COLOR : SELECTED_COLOR);

            // Value text
            tft.setTextSize(1);
            tft.setFont(&FreeMonoBold9pt7b);
            tft.setCursor(190, yPos + 15);
            tft.print(rgbState.values[i]);
            if (i == 3)
                tft.print("%");
        }

        // Help text
        tft.setTextSize(1);
        tft.setFont();
        tft.setTextColor(MUTED_COLOR);
        tft.setCursor(15, 250);
        tft.print(ui_rotate_adjust);
        tft.setCursor(15, 258);
        tft.print(ui_press_next);
        tft.setCursor(15, 266);
        tft.print(ui_long_press_quit);

        rgbState.needsRefresh = false;
    }
}

void displayClockSubmenu(void *parameters)
{
    static int lastSelectedOption = -1;
    needsFullRedraw = true;

    if (needsFullRedraw)
    {
        tft.fillScreen(BG_COLOR);
        needsFullRedraw = false;
    }

    // Draw title using same style as settings
    renderScreenTitle(ui_clock);

    // Menu configuration similar to settings menu
    const int MENU_START_Y = 62;
    const int ITEM_SPACING = 54;
    const char *labels[] = {ui_hours, ui_minutes, ui_seconds};
    const int values[] = {hours, minutes, seconds};

    // Draw menu items with consistent styling
    for (int i = 0; i < 3; i++)
    {
        bool selected = (i == settingsSelectedOption);
        int baseY = MENU_START_Y + (i * ITEM_SPACING);

        // Draw the button without right arrow since we're showing values
        drawButton(labels[i], nullptr, 0, 0, false, baseY, selected);

        // Add the values (they're positioned differently from the right arrow)
        char valueStr[3];
        sprintf(valueStr, "%02d", values[i]);
        tft.setTextSize(2);
        tft.setFont();
        tft.setTextColor(selected ? 0x0 : TEXT_COLOR);
        tft.setCursor(180, baseY + (selected ? 17 : 15));
        tft.print(valueStr);
    }

    // Footer text with settings-style formatting
    tft.setTextSize(1);
    tft.setFont();
    tft.setTextColor(ULTRA_MUTED_COLOR);
    tft.setCursor(76, 287);
    tft.print(ui_secured_by);
    tft.setTextColor(MUTED_COLOR);
    tft.setCursor(65, 296);
    tft.print(ui_powered_by);
    tft.setCursor(79, 305);
    tft.print(ui_build);
    tft.print(" ");
    tft.print(OS_version);

    lastSelectedOption = settingsSelectedOption;
}