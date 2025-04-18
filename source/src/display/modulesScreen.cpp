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
extern bool moduleConnectionStatus;
extern const char *connectedModuleName;

void displayModulesSubmenu(void *parameters)
{
    static unsigned long lastCheckTime = 0;
    const unsigned long CHECK_INTERVAL = 500; // Check state every 500ms
    static bool animationState = false;
    static bool previousConnectionStatus = !moduleConnectionStatus; // Initialize to opposite to force first draw
    static const char *previousModuleName = "";

    // Check if we need to update due to state changes
    bool stateChanged = (moduleConnectionStatus != previousConnectionStatus) ||
                        (moduleConnectionStatus && strcmp(connectedModuleName, previousModuleName) != 0);
    bool shouldCheck = (millis() - lastCheckTime >= CHECK_INTERVAL);

    // Only refresh if there are actual changes
    bool shouldRefresh = needsFullRedraw || stateChanged;

    if (shouldRefresh)
    {
        if (needsFullRedraw)
        {
            tft.fillScreen(BG_COLOR);
            drawScreenTitle(ui_modules);
            needsFullRedraw = false;
        }

        // Clear the menu area
        tft.fillRect(4, 60, 232, 215, BG_COLOR);

        // Update animation state        // Update animation state if it's time to check
        if (shouldCheck)
        {
            animationState = !animationState;
            lastCheckTime = millis();

            // Store current state for next comparison
            previousConnectionStatus = moduleConnectionStatus;
            if (moduleConnectionStatus)
            {
                previousModuleName = connectedModuleName;
            }
        }

        if (!moduleConnectionStatus)
        {
            // Draw a decorative box for the disconnected state
            drawFrame(15, 80, 210, 120, ERROR_COLOR);

            // Draw BLE icon with animation
            tft.drawBitmap(107, 95, iconBleDisconnected, 14, 15,
                           animationState ? ERROR_COLOR : MUTED_COLOR, BG_COLOR);

            // Display "No module" message
            tft.setTextSize(2);
            tft.setFont();
            tft.setTextColor(ERROR_COLOR);
            tft.setCursor(65, 125);
            tft.print(ui_no_module);

            // Display connection instructions
            tft.setTextSize(1);
            tft.setTextColor(MUTED_COLOR);
            tft.setCursor(35, 150);
            tft.print(ui_no_module_desc);
        }
        else
        {
            // Draw a decorative box for the connected state
            drawFrame(15, 80, 210, 120, SUCCESS_COLOR);

            // Draw BLE icon with animation
            tft.drawBitmap(25, 95, iconBleConnected, 14, 15,
                           animationState ? SUCCESS_COLOR : HIGHLIGHT_COLOR, BG_COLOR);

            // Display connection info
            tft.setTextSize(2);
            tft.setFont();
            tft.setTextColor(TEXT_COLOR);
            tft.setCursor(45, 95);
            tft.print(ui_connected_to);

            // Display module name with highlight
            tft.setTextSize(2);
            tft.setTextColor(HIGHLIGHT_COLOR);
            int nameX = (210 - (strlen(connectedModuleName) * 12)) / 2 + 15;
            tft.setCursor(nameX, 125);
            tft.print(connectedModuleName);

            // Display status with animated dot
            tft.setTextSize(1);
            tft.setTextColor(SUCCESS_COLOR);
            tft.setCursor(25, 165);
            tft.print(ui_module_status);
            tft.fillCircle(110, 169, 2, animationState ? SUCCESS_COLOR : BG_COLOR);
        }

        // Draw help text
        tft.setTextSize(1);
        tft.setFont();
        tft.setTextColor(MUTED_COLOR);
        tft.setCursor(15, 250);
        tft.print(ui_long_press_quit);

        // Draw footer text
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
}
