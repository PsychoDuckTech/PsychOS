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
    // Define options for the RGB menu
    const char *rgbOptions[] = {ui_brightness, "Speed"};
    const int valueRanges[] = {100, 20}; // Brightness 0-100%, Speed 1-20
    
    // Sync with actual values from RGB task only on initial draw or when explicitly needed
    if (needsFullRedraw) {
        // We already synced when switching to the screen, but grab direct values if needed
        // for additional verification
        uint8_t actualBrightness, actualSpeed;
        if (uRGB.getCurrentValues(&actualBrightness, &actualSpeed)) {
            // Update rgbState with actual values if they differ
            if (rgbState.brightness != actualBrightness || rgbState.speed != actualSpeed) {
                rgbState.brightness = actualBrightness;
                rgbState.speed = actualSpeed;
                rgbState.needsRefresh = true;
            }
        }
    }
    
    // Only do a full redraw when necessary
    if (needsFullRedraw || rgbState.needsRefresh)
    {
        // Clear the screen only on full redraws
        if (needsFullRedraw) {
            tft.fillScreen(BG_COLOR);
            needsFullRedraw = false;
        }

        // Title using universal function
        drawScreenTitle(ui_underglow, 30);

        // Define the options area dimensions
        const int OPTIONS_AREA_TOP = 60;
        const int OPTIONS_AREA_HEIGHT = 180;
        
        // Clear the options area with a black rectangle (ensures clean refresh)
        tft.fillRect(0, OPTIONS_AREA_TOP, tft.width(), OPTIONS_AREA_HEIGHT, BG_COLOR);

        // Draw both slider buttons with appropriate icons
        const uint8_t *icons[] = {iconLightBulb, iconMusicPlay}; // Reusing existing icons
        const int iconWidths[] = {18, 20};
        const int iconHeights[] = {23, 18};
        
        // Define Y positions for the buttons
        const int MENU_START_Y = 80;
        const int ITEM_SPACING = 60;
        
        // Current values to display - using the synchronized values from rgbState
        int currentValues[] = {rgbState.brightness, rgbState.speed};
        
        // Draw the buttons
        for (int i = 0; i < 2; i++)
        {
            bool selected = (i == rgbState.currentSelection);
            int yPos = MENU_START_Y + (i * ITEM_SPACING);
            
            // Draw slider button with appropriate parameters
            drawSliderButton(rgbOptions[i], currentValues[i], valueRanges[i], 
                           icons[i], iconWidths[i], iconHeights[i], 
                           (i == 0), // Only show percentage for brightness
                           yPos, selected);
        }

        // Help text at the bottom of the screen
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