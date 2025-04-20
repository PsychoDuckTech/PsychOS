#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include "display/displayContent.h"
#include "globals.h"
#include "display/icons.h"
#include "main.h"
#include "translations.h"
#include "display/components/uiComponents.h"
#include "tasks/displayHandler.h"

extern Adafruit_ILI9341 tft;
extern bool needsFullRedraw;
extern int settingsSelectedOption;
extern int hours;
extern int minutes;
extern int seconds;

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
    drawScreenTitle(ui_clock);

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

    // Draw footer
    drawFooter();

    lastSelectedOption = settingsSelectedOption;
}