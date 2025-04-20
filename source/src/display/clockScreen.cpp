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

    // Draw non-selected menu items first
    for (int i = 0; i < 3; i++)
    {
        if (i != settingsSelectedOption)
        {
            int baseY = MENU_START_Y + (i * ITEM_SPACING);

            // Draw the button without right arrow since we're showing values
            drawButton(labels[i], nullptr, 0, 0, false, baseY, false);

            // Add the values (they're positioned differently from the right arrow)
            char valueStr[3];
            sprintf(valueStr, "%02d", values[i]);
            tft.setTextSize(2);
            tft.setFont();
            tft.setTextColor(TEXT_COLOR);
            tft.setCursor(180, baseY + 15);
            tft.print(valueStr);
        }
    }

    // Then draw the selected item last for perceived performance improvement
    int baseY = MENU_START_Y + (settingsSelectedOption * ITEM_SPACING);

    // Draw the selected button without right arrow
    drawButton(labels[settingsSelectedOption], nullptr, 0, 0, false, baseY, true);

    // Add the value for the selected item
    char valueStr[3];
    sprintf(valueStr, "%02d", values[settingsSelectedOption]);
    tft.setTextSize(2);
    tft.setFont();
    tft.setTextColor(0x0); // Black text for selected item
    tft.setCursor(180, baseY + 17);
    tft.print(valueStr);

    // Draw footer
    drawFooter();

    // Draw help indicators
    drawHelpIndicators(230, MUTED_COLOR);

    lastSelectedOption = settingsSelectedOption;
}