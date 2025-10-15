#include <Adafruit_ILI9341.h>
#include <Arduino.h>
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
extern int settingsScrollOffset;

// Menu item structure for settings
struct SettingsMenuItem
{
    const char *text;
    const uint8_t *icon;
    int iconWidth;
    int iconHeight;
};

// Define all settings menu items here - easy to add more!
const SettingsMenuItem settingsMenuItems[] = {
    {ui_modules, iconBleConnectedBig, 14, 22},
    {ui_underglow, iconLightBulb, 18, 23},
    {ui_clock, iconTranslation, 22, 22},
    {ui_pixel_flush, nullptr, 0, 0},
    {ui_iot_link, nullptr, 0, 0},
    // Test items to demonstrate scrolling (can be removed later)
    {ui_build, nullptr, 0, 0},
    {ui_brightness, nullptr, 0, 0}
};

const int settingsMenuItemCount = sizeof(settingsMenuItems) / sizeof(settingsMenuItems[0]);

int getSettingsMenuItemCount()
{
    return settingsMenuItemCount;
}

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
    
    const int MENU_START_Y = 62;
    const int ITEM_SPACING = 54;
    const int MAX_VISIBLE_ITEMS = 4; // Maximum items that can fit on screen
    
    // Calculate which items are visible based on scroll offset
    int visibleStartIdx = settingsScrollOffset;
    int visibleEndIdx = min(visibleStartIdx + MAX_VISIBLE_ITEMS, settingsMenuItemCount);
    
    // Draw all non-selected visible items first
    for (int i = visibleStartIdx; i < visibleEndIdx; i++)
    {
        if (i != settingsSelectedOption)
        {
            int displayIndex = i - visibleStartIdx; // Position on screen (0-3)
            int baseY = MENU_START_Y + (displayIndex * ITEM_SPACING);
            
            drawButton(settingsMenuItems[i].text, 
                      settingsMenuItems[i].icon, 
                      settingsMenuItems[i].iconWidth, 
                      settingsMenuItems[i].iconHeight, 
                      true, baseY, false);
        }
    }
    
    // Draw the selected item last for perceived performance improvement
    if (settingsSelectedOption >= visibleStartIdx && settingsSelectedOption < visibleEndIdx)
    {
        int displayIndex = settingsSelectedOption - visibleStartIdx;
        int baseY = MENU_START_Y + (displayIndex * ITEM_SPACING);
        
        drawButton(settingsMenuItems[settingsSelectedOption].text,
                   settingsMenuItems[settingsSelectedOption].icon,
                   settingsMenuItems[settingsSelectedOption].iconWidth,
                   settingsMenuItems[settingsSelectedOption].iconHeight,
                   true, baseY, true);
    }
}