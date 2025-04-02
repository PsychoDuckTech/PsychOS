#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include "display/displayContent.h"
#include "globals.h"
#include "display/icons.h"
#include "main.h"
#include "translations.h"
#include "display/components/uiComponents.h"
#include "tasks/rgbHandler.h"

extern Adafruit_ILI9341 tft;
extern bool needsFullRedraw;
extern int rgbValues[4];

void displayRGBSubmenu(void *parameters)
{
    const char *rgbOptions[] = {ui_red, ui_green, ui_blue, ui_brightness};
    const int valueRanges[] = {255, 255, 255, 100};
    needsFullRedraw = true;

    if (rgbState.needsRefresh)
    {
        tft.fillScreen(BG_COLOR);

        // Title using universal function
        drawScreenTitle(ui_underglow, 30);

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