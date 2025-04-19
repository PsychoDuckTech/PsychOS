#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include "display/displayContent.h"
#include "globals.h"
#include "display/icons.h"
#include "main.h"
#include "translations.h"
#include "display/components/uiComponents.h"
#include "tasks/displayHandler.h"
#include "tasks/BLEHandler.h"

extern Adafruit_ILI9341 tft;
extern bool needsFullRedraw;
extern bool moduleConnectionStatus;
extern const char *connectedModuleName;

void displayModulesSubmenu(void *parameters)
{
    static bool previousConnectionStatus = !moduleConnectionStatus;
    static const char *previousModuleName = "";
    static int previousKeyPresses = -1;
    static int previousRSSI = 0;
    static unsigned long lastTimeCheck = 0;
    static unsigned long lastRSSICheck = 0;
    static unsigned long previousSeconds = 0;

    unsigned long currentMillis = millis();
    unsigned long currentSeconds = currentMillis / 1000;

    // Check for periodic updates (every 100ms)
    bool timeToUpdate = (currentMillis - lastTimeCheck) >= 100;
    if (timeToUpdate)
    {
        lastTimeCheck = currentMillis;
    }

    // Check RSSI every 500ms
    bool timeToCheckRSSI = (currentMillis - lastRSSICheck) >= 500;
    if (timeToCheckRSSI)
    {
        lastRSSICheck = currentMillis;
    }

    // Check if module is actually connected by querying BLE status
    bool actuallyConnected = BLE.connected();
    if (moduleConnectionStatus != actuallyConnected)
    {
        moduleConnectionStatus = actuallyConnected; // Update the global state
    }

    // Find current stats to check for changes
    ModuleStat *currentStats = nullptr;
    String currentAddress = "";
    if (moduleConnectionStatus)
    {
        currentAddress = BLE.central().address();
        for (int i = 0; i < numModules; i++)
        {
            if (moduleStats[i].address == currentAddress)
            {
                currentStats = &moduleStats[i];
                if (timeToCheckRSSI)
                {
                    // Update RSSI periodically
                    currentStats->rssi = BLE.central().rssi();
                }
                break;
            }
        }
    }

    bool statsChanged = false;
    if (currentStats)
    {
        statsChanged = (currentStats->keyPresses != previousKeyPresses) ||
                       (currentStats->rssi != previousRSSI) ||
                       (currentSeconds != previousSeconds); // Check if time has changed

        if (statsChanged)
        {
            previousKeyPresses = currentStats->keyPresses;
            previousRSSI = currentStats->rssi;
            previousSeconds = currentSeconds;
        }
    }

    bool stateChanged = (moduleConnectionStatus != previousConnectionStatus) ||
                        (moduleConnectionStatus && strcmp(connectedModuleName, previousModuleName) != 0) ||
                        statsChanged ||
                        timeToUpdate; // Force refresh on periodic updates
    bool shouldRefresh = needsFullRedraw || stateChanged;

    if (shouldRefresh)
    {
        if (needsFullRedraw)
        {
            tft.fillScreen(BG_COLOR);
            drawScreenTitle(ui_modules);
            needsFullRedraw = false;
        }

        tft.fillRect(4, 60, 232, 215, BG_COLOR);

        previousConnectionStatus = moduleConnectionStatus;
        if (moduleConnectionStatus)
        {
            previousModuleName = connectedModuleName;
        }

        if (!moduleConnectionStatus)
        {
            drawFrame(15, 80, 210, 120, ERROR_COLOR, 1);
            tft.drawBitmap(107, 95, iconBleDisconnected, 14, 15,
                           ERROR_COLOR, BG_COLOR);

            tft.setTextSize(2);
            tft.setFont();
            tft.setTextColor(ERROR_COLOR);
            tft.setCursor(65, 125);
            tft.print(ui_no_module);

            tft.setTextSize(1);
            tft.setTextColor(MUTED_COLOR);
            tft.setCursor(35, 150);
            tft.print(ui_no_module_desc);
        }
        else
        {
            // Find the stats for the current module
            ModuleStat *currentStats = nullptr;
            String currentAddress = BLE.connected() ? BLE.central().address() : "";
            for (int i = 0; i < numModules; i++)
            {
                if (moduleStats[i].address == currentAddress)
                {
                    currentStats = &moduleStats[i];
                    break;
                }
            }

            // Main connection info box (full width)
            drawFrame(15, 80, 210, 70, SUCCESS_COLOR, 1);

            // Draw BLE icon
            tft.drawBitmap(25, 90, iconBleConnected, 14, 15,
                           SUCCESS_COLOR, BG_COLOR);

            // Display connection info and module name
            tft.setTextSize(2);
            tft.setFont();
            tft.setTextColor(TEXT_COLOR);
            int nameX = (210 - (strlen(connectedModuleName) * 12)) / 2 + 15;
            tft.setCursor(nameX, 95);
            tft.print(connectedModuleName);

            if (currentStats)
            {
                // Display centered connection time
                tft.setTextSize(1);
                unsigned long duration = (millis() - currentStats->connectTime) / 1000;
                char timeStr[20];
                if (duration < 60)
                {
                    sprintf(timeStr, "%lds", duration);
                }
                else if (duration < 3600)
                {
                    sprintf(timeStr, "%ldm %lds", duration / 60, duration % 60);
                }
                else
                {
                    sprintf(timeStr, "%ldh %ldm", duration / 3600, (duration % 3600) / 60);
                }

                // Center the connection time text
                int16_t x1, y1;
                uint16_t w, h;
                String connText = "Connected: " + String(timeStr);
                tft.getTextBounds(connText.c_str(), 0, 0, &x1, &y1, &w, &h);
                int textX = 15 + (210 - w) / 2;

                tft.setCursor(textX, 125);
                tft.setTextColor(TEXT_COLOR);
                tft.print("Connected: ");
                tft.setTextColor(HIGHLIGHT_COLOR);
                tft.print(timeStr);

                // Two boxes side by side for stats
                // Left box - Key Presses
                drawFrame(15, 160, 100, 60, TEXT_COLOR, 0);
                tft.setTextSize(1);

                // Center "Key Presses" text
                tft.getTextBounds("Key Presses", 0, 0, &x1, &y1, &w, &h);
                textX = 15 + (100 - w) / 2;
                tft.setCursor(textX, 175);
                tft.setTextColor(TEXT_COLOR);
                tft.print("Key Presses");

                // Center the number
                String keypressStr = String(currentStats->keyPresses);
                tft.getTextBounds(keypressStr.c_str(), 0, 0, &x1, &y1, &w, &h);
                textX = 15 + (100 - w) / 2;
                tft.setCursor(textX, 195);
                tft.setTextColor(HIGHLIGHT_COLOR);
                tft.print(keypressStr);

                // Right box - Signal Strength
                drawFrame(125, 160, 100, 60, TEXT_COLOR, 0);

                // Center "Signal" text
                tft.getTextBounds("Signal", 0, 0, &x1, &y1, &w, &h);
                textX = 125 + (100 - w) / 2;
                tft.setCursor(textX, 175);
                tft.setTextColor(TEXT_COLOR);
                tft.print("Signal");

                // Center the RSSI value
                String rssiStr = String(currentStats->rssi) + " dBm";
                tft.getTextBounds(rssiStr.c_str(), 0, 0, &x1, &y1, &w, &h);
                textX = 125 + (100 - w) / 2;
                tft.setCursor(textX, 195);
                tft.setTextColor(HIGHLIGHT_COLOR);
                tft.print(rssiStr);

                // Connection status
                String statusText = "Status: Connected";
                tft.getTextBounds(statusText.c_str(), 0, 0, &x1, &y1, &w, &h);
                textX = 15 + (210 - w) / 2;
                tft.setCursor(textX, 235);
                tft.setTextColor(SUCCESS_COLOR);
                tft.print(statusText);
            }
        }

        // Draw help text
        tft.setTextSize(1);
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
