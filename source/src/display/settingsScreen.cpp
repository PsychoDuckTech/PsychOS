#include <Adafruit_ILI9341.h>
#include "arduino_compat.h"
#include "display/screens.h"
#include "display/displayContent.h"
#include "globals.h"
#include "display/icons.h"
#include "main.h"
#include "translations.h"
#include "display/components/uiComponents.h"
#include "tasks/displayHandler.h"

extern SemaphoreHandle_t screenMutex;
extern Adafruit_ILI9341 tft;
extern bool needsFullRedraw;
extern int settingsSelectedOption;

// Forward declarations for menu actions
void actionModules();
void actionUnderglow();
void actionClock();
void actionPixelFlush();
void actionLanguages();
void actionAbout();

// Settings menu configuration - defined in uiComponents.h
const ScrollableMenuItem settingsMenuItems[] = {
    {ui_modules, iconBleConnectedBig, 14, 22, actionModules},
    {ui_underglow, iconLightBulb, 18, 23, actionUnderglow},
    {ui_clock, iconTranslation, 22, 22, actionClock},
    {ui_pixel_flush, nullptr, 0, 0, actionPixelFlush},
    {ui_languages, nullptr, 0, 0, actionLanguages},
    {ui_about, iconInfo, 14, 22, actionAbout}
};

const int SETTINGS_MENU_ITEM_COUNT = 6;
const int MAX_VISIBLE_ITEMS = 4;

void drawSettingsStatic(void *parameters)
{
    tft.fillScreen(BG_COLOR);
    // Draw title
    drawScreenTitle(ui_settings);

    // Draw footer
    drawFooter();
}

void displaySettingsScreen(void *parameters)
{
    if (!needsFullRedraw)
    {
        drawSettingsStatic(parameters);
        needsFullRedraw = false;
    }
    else
    {
        // Only clear the menu area, not the title and footer
        tft.fillRect(4, 60, 232, 215, BG_COLOR);
    }

    const int MENU_START_Y = 48;  // Moved up by 14 pixels
    const int ITEM_SPACING = 54;

    // Calculate current page based on selected option
    int currentPage = settingsSelectedOption / MAX_VISIBLE_ITEMS;
    int startIndex = currentPage * MAX_VISIBLE_ITEMS;
    int endIndex = min(startIndex + MAX_VISIBLE_ITEMS, SETTINGS_MENU_ITEM_COUNT);
    int itemsOnPage = endIndex - startIndex;

    // Calculate total pages
    int totalPages = (SETTINGS_MENU_ITEM_COUNT + MAX_VISIBLE_ITEMS - 1) / MAX_VISIBLE_ITEMS; // Ceiling division

    // Draw pagination dots at the bottom
    if (totalPages > 1) {
        const int DOT_SPACING = 12;
        const int DOT_RADIUS = 3;
        const int DOT_Y = MENU_START_Y + (MAX_VISIBLE_ITEMS * ITEM_SPACING) + 9;  // Moved up by 20 pixels from original position
        int totalDotsWidth = (totalPages - 1) * DOT_SPACING;
        int startX = (tft.width() - totalDotsWidth) / 2;

        for (int i = 0; i < totalPages; i++) {
            int dotX = startX + (i * DOT_SPACING);
            if (i == currentPage) {
                // Filled dot for current page
                tft.fillCircle(dotX, DOT_Y, DOT_RADIUS, TEXT_COLOR);
            } else {
                // Outline dot for other pages
                tft.drawCircle(dotX, DOT_Y, DOT_RADIUS, TEXT_COLOR);
            }
        }
    }

    // First, draw all non-selected items for perceived performance improvement
    for (int i = 0; i < itemsOnPage; i++)
    {
        int itemIndex = startIndex + i;
        if (itemIndex != settingsSelectedOption)
        {
            int baseY = MENU_START_Y + (i * ITEM_SPACING);
            const ScrollableMenuItem &item = settingsMenuItems[itemIndex];
            drawButton(item.text, item.icon, item.iconWidth, item.iconHeight, true, baseY, false);
        }
    }

    // Then, draw the selected item last for perceived performance improvement
    for (int i = 0; i < itemsOnPage; i++)
    {
        int itemIndex = startIndex + i;
        if (itemIndex == settingsSelectedOption)
        {
            int baseY = MENU_START_Y + (i * ITEM_SPACING);
            const ScrollableMenuItem &item = settingsMenuItems[itemIndex];
            drawButton(item.text, item.icon, item.iconWidth, item.iconHeight, true, baseY, true);
            break; // Only one selected item
        }
    }
}

// Menu action implementations - centralized in one place
void actionModules() {
    switchScreen(ModulesSubmenu);
}

void actionUnderglow() {
    switchScreen(RGBLightingSubmenu);
}

void actionClock() {
    settingsSelectedOption = 0;
    switchScreen(ClockSubmenu);
}

void actionPixelFlush() {
    pixelFlushComplete = false;
    switchScreen(PixelFlushScreen);
}

void actionLanguages() {
    // Languages menu - placeholder for now
}

void actionAbout() {
    // About menu - placeholder for now
}