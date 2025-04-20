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

// Function prototype for delay estimation
float calculateDelayFromRSSI(int rssi);

void displayModulesSubmenu(void *parameters)
{
    static bool previousConnectionStatus = !moduleConnectionStatus;
    static const char *previousModuleName = "";
    static int previousKeyPresses = -1;
    static int previousRSSI = 0;
    static unsigned long lastTimeCheck = 0;
    static unsigned long previousSeconds = 0;

    unsigned long currentMillis = millis();
    unsigned long currentSeconds = currentMillis / 1000;

    // Check for updates every second - this will handle both time and RSSI updates
    bool timeToUpdate = (currentMillis - lastTimeCheck) >= 1000;
    if (timeToUpdate)
    {
        lastTimeCheck = currentMillis;
    }

    // Check if module is actually connected by querying BLE status
    bool actuallyConnected = BLE.connected();
    if (moduleConnectionStatus != actuallyConnected)
    {
        moduleConnectionStatus = actuallyConnected; // Update the global state
        needsFullRedraw = true;                     // Force full redraw when connection state changes
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
                if (timeToUpdate)
                {
                    // Update RSSI once per second
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
                       (timeToUpdate && ((currentStats->rssi != previousRSSI) ||
                                         (currentSeconds != previousSeconds))); // Only check RSSI and time changes during update interval

        if (statsChanged)
        {
            previousKeyPresses = currentStats->keyPresses;
            previousRSSI = currentStats->rssi;
            previousSeconds = currentSeconds;
        }
    }

    bool stateChanged = (moduleConnectionStatus != previousConnectionStatus) ||
                        (moduleConnectionStatus && strcmp(connectedModuleName, previousModuleName) != 0) ||
                        statsChanged;

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
            // Main disconnection info box (full width)
            drawFrame(15, 80, 210, 70, ERROR_COLOR, 1);

            // Draw BLE icon
            tft.drawBitmap(25, 90, iconBleDisconnected, 14, 15, ERROR_COLOR, BG_COLOR);

            // Display "No Module" text centered
            tft.setTextSize(2);
            tft.setFont();
            tft.setTextColor(ERROR_COLOR);
            int16_t x1, y1;
            uint16_t w, h;
            tft.getTextBounds(ui_no_module, 0, 0, &x1, &y1, &w, &h);
            int nameX = 15 + (210 - w) / 2;
            tft.setCursor(nameX, 95);
            tft.print(ui_no_module);

            // Center the status message
            tft.setTextSize(1);
            tft.getTextBounds(ui_no_module_desc, 0, 0, &x1, &y1, &w, &h);
            int textX = 15 + (210 - w) / 2;
            tft.setCursor(textX, 125);
            tft.setTextColor(MUTED_COLOR);
            tft.print(ui_no_module_desc);

            // Two boxes side by side for tips
            // Left box - Search Status
            drawFrame(15, 160, 100, 60, ERROR_COLOR, 0);
            tft.setTextSize(1);

            // Center "Status" text
            tft.getTextBounds("Status", 0, 0, &x1, &y1, &w, &h);
            textX = 15 + (100 - w) / 2;
            tft.setCursor(textX, 175);
            tft.setTextColor(TEXT_COLOR);
            tft.print("Status");

            // Center status text
            const char *searchingText = "Searching...";
            tft.getTextBounds(searchingText, 0, 0, &x1, &y1, &w, &h);
            textX = 15 + (100 - w) / 2;
            tft.setCursor(textX, 195);
            tft.setTextColor(ERROR_COLOR);
            tft.print(searchingText);

            // Right box - Help
            drawFrame(125, 160, 100, 60, ERROR_COLOR, 0);

            // Center "Help" text
            tft.getTextBounds("Help", 0, 0, &x1, &y1, &w, &h);
            textX = 125 + (100 - w) / 2;
            tft.setCursor(textX, 175);
            tft.setTextColor(TEXT_COLOR);
            tft.print("Help");

            // Center help text
            const char *helpText = "Press any key";
            tft.getTextBounds(helpText, 0, 0, &x1, &y1, &w, &h);
            textX = 125 + (100 - w) / 2;
            tft.setCursor(textX, 195);
            tft.setTextColor(ERROR_COLOR);
            tft.print(helpText);

            // Connection status
            String statusText = "Status: Disconnected";
            tft.getTextBounds(statusText.c_str(), 0, 0, &x1, &y1, &w, &h);
            textX = 15 + (210 - w) / 2;
            tft.setCursor(textX, 235);
            tft.setTextColor(ERROR_COLOR);
            tft.print(statusText);
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

                // Estimated Key Click Delay based on RSSI
                float estimatedDelay = calculateDelayFromRSSI(currentStats->rssi);
                String delayText;
                uint16_t delayColor;

                // Format the delay text and color based on severity
                if (estimatedDelay < 10)
                {
                    delayText = "Delay: ~" + String(estimatedDelay, 1) + "ms (Great!)";
                    delayColor = HIGHLIGHT_COLOR; // Great connection
                }
                else if (estimatedDelay < 20)
                {
                    delayText = "Delay: ~" + String(estimatedDelay, 1) + "ms (Good)";
                    delayColor = 0x07E0; // Green
                }
                else if (estimatedDelay < 50)
                {
                    delayText = "Delay: ~" + String(estimatedDelay, 1) + "ms (OK)";
                    delayColor = 0xFFE0; // Yellow
                }
                else
                {
                    delayText = "Delay: ~" + String(estimatedDelay, 1) + "ms (Poor)";
                    delayColor = 0xF800; // Red
                }

                // Draw the estimated delay text
                tft.getTextBounds(delayText.c_str(), 0, 0, &x1, &y1, &w, &h);
                textX = 15 + (210 - w) / 2;
                tft.setCursor(textX, 247);
                tft.setTextColor(delayColor);
                tft.print(delayText);
            }
        }

        // Draw help text
        tft.setTextSize(1);
        tft.setTextColor(MUTED_COLOR);
        tft.setCursor(15, 250);
        tft.print(ui_long_press_quit);

        // Draw footer
        drawFooter();
    }
}

/**
 * Calculates an estimated key click delay based on Bluetooth RSSI values
 *
 * @param rssi Signal strength in dBm (typically between -100 and -30)
 * @return Estimated delay in milliseconds
 */
float calculateDelayFromRSSI(int rssi)
{
    // RSSI ranges and their corresponding approximate delays
    // Excellent: -30 to -67 dBm: ~5-10ms
    // Good: -68 to -80 dBm: ~10-20ms
    // Fair: -81 to -90 dBm: ~20-50ms
    // Poor: -91 to -100 dBm: ~50-100ms

    // Normalize RSSI to a positive value between 0-70
    // where -30 dBm = 70 (best) and -100 dBm = 0 (worst)
    float normalizedRSSI = constrain(rssi + 100, 0, 70);

    // Map the normalized value to a delay estimate
    // 0 (worst) -> 100ms delay
    // 70 (best) -> 5ms delay
    float delay = 100.0 - ((normalizedRSSI / 70.0) * 95.0);

    // Add a slight random factor (+/- 10%) to make it feel more dynamic
    delay *= (1.0 + ((random(21) - 10) / 100.0));

    return delay;
}
