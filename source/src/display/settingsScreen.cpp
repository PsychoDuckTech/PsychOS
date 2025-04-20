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
    if (needsFullRedraw)
    {
        drawSettingsStatic(parameters);
        needsFullRedraw = false;
    }
    tft.fillRect(4, 60, 232, 215, BG_COLOR);
    const int MENU_START_Y = 62;
    const int ITEM_SPACING = 54;
    const char *menuItems[] = {ui_modules, ui_underglow, ui_clock, ui_pixel_flush};
    const uint8_t *icons[] = {iconBleConnectedBig, iconLightBulb, iconTranslation, nullptr};
    const int iconWidths[] = {14, 18, 22, 22, 0};
    const int iconHeights[] = {22, 23, 22, 22, 0};
    for (int i = 0; i < 4; i++)
    {
        bool selected = (i == settingsSelectedOption);
        int baseY = MENU_START_Y + (i * ITEM_SPACING);
        drawButton(menuItems[i], icons[i], iconWidths[i], iconHeights[i], true, baseY, selected);
    }
}