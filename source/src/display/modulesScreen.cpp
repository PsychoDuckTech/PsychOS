#include <Adafruit_ILI9341.h>
#include "arduino_compat.h"
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
int calculateDelayFromRSSI(int rssi);

void displayModulesSubmenu(void *parameters)
{
    static bool previousConnectionStatus = !moduleConnectionStatus;
    static const char *previousModuleName = "";
    static int previousKeyPresses = -1;
    static int previousRSSI = 0;
    static unsigned long lastTimeCheck = 0;
    static unsigned long previousSeconds = 0;
    static bool staticElementsDrawn = false;

    // Declare textX once for the entire function
    int textX;

    // Declare text bounds variables once for the entire function
    int16_t x1, y1;
    uint16_t w, h;

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
        staticElementsDrawn = false;                // Force static elements to be redrawn
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
                        (moduleConnectionStatus && strcmp(connectedModuleName, previousModuleName) != 0);

    bool shouldRefresh = needsFullRedraw || stateChanged;
    bool shouldUpdateDynamicContent = shouldRefresh || statsChanged;

    // Handle full screen redraw if necessary
    if (needsFullRedraw)
    {
        tft.fillScreen(BG_COLOR);
        drawScreenTitle(ui_modules);
        staticElementsDrawn = false; // Force static elements to be redrawn
        needsFullRedraw = false;
    }

    // Update connection status variables
    if (stateChanged)
    {
        previousConnectionStatus = moduleConnectionStatus;
        if (moduleConnectionStatus)
        {
            previousModuleName = connectedModuleName;
        }
        staticElementsDrawn = false; // Force static elements to be redrawn when state changes
    }

    // DISCONNECTED STATE RENDERING
    if (!moduleConnectionStatus)
    {
        // Draw static elements (frames, titles, icons) only when needed
        if (!staticElementsDrawn)
        {
            tft.fillRect(4, 60, 232, 215, BG_COLOR);

            // Main disconnection info box (full width)
            drawFrame(15, 80, 210, 70, ERROR_COLOR, 1);

            // Draw BLE icon
            tft.drawBitmap(25, 90, iconBleDisconnected, 14, 15, ERROR_COLOR, BG_COLOR);

            // Display "No Module" text centered
            tft.setTextSize(2);
            tft.setFont();
            tft.setTextColor(ERROR_COLOR);
            tft.getTextBounds(ui_no_module, 0, 0, &x1, &y1, &w, &h);
            int nameX = 15 + (210 - w) / 2;
            tft.setCursor(nameX, 95);
            tft.print(ui_no_module);

            // Center the status message
            tft.setTextSize(1);
            tft.getTextBounds(ui_no_module_desc, 0, 0, &x1, &y1, &w, &h);
            textX = 15 + (210 - w) / 2;
            tft.setCursor(textX, 125);
            tft.setTextColor(MUTED_COLOR);
            tft.print(ui_no_module_desc);

            // Two boxes side by side for tips
            // Left box - Search Status
            drawFrame(15, 160, 100, 60, ERROR_COLOR, 0);
            tft.setTextSize(1);

            // Center "Status" text
            tft.setTextSize(1);
            tft.getTextBounds(ui_status, 0, 0, &x1, &y1, &w, &h);
            textX = 15 + (100 - w) / 2;
            tft.setCursor(textX, 175);
            tft.setTextColor(TEXT_COLOR);
            tft.print(ui_status);

            // Right box - Help
            drawFrame(125, 160, 100, 60, ERROR_COLOR, 0);

            // Center "Help" text
            tft.getTextBounds(ui_help, 0, 0, &x1, &y1, &w, &h);
            textX = 125 + (100 - w) / 2;
            tft.setCursor(textX, 175);
            tft.setTextColor(TEXT_COLOR);
            tft.print(ui_help);

            // Draw help text
            tft.setTextSize(1);
            tft.setTextColor(MUTED_COLOR);
            tft.setCursor(15, 250);
            tft.print(ui_long_press_quit);

            // Draw footer
            drawFooter();

            staticElementsDrawn = true;
        }

        // Dynamic content - always updated when refresh is needed
        if (shouldUpdateDynamicContent)
        {
            // Clear the dynamic text areas only (inset by 15% from each side)
            tft.fillRect(30, 195, 70, 15, BG_COLOR);  // Status text area (inside left box)
            tft.fillRect(140, 195, 70, 15, BG_COLOR); // Help text area (inside right box)
            tft.fillRect(45, 235, 150, 15, BG_COLOR); // Connection status area

            // Center status text
            tft.setTextSize(1);
            tft.getTextBounds(ui_searching, 0, 0, &x1, &y1, &w, &h);
            textX = 15 + (100 - w) / 2;
            tft.setCursor(textX, 195);
            tft.setTextColor(ERROR_COLOR);
            tft.print(ui_searching);

            // Center help text
            tft.getTextBounds(ui_contact_us, 0, 0, &x1, &y1, &w, &h);
            textX = 125 + (100 - w) / 2;
            tft.setCursor(textX, 195);
            tft.setTextColor(ERROR_COLOR);
            tft.print(ui_contact_us);

            // Connection status
            tft.getTextBounds(ui_status_disconnected, 0, 0, &x1, &y1, &w, &h);
            textX = 15 + (210 - w) / 2;
            tft.setCursor(textX, 235);
            tft.setTextColor(ERROR_COLOR);
            tft.print(ui_status_disconnected);
        }
    }
    // CONNECTED STATE RENDERING
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

        // Only draw static elements when needed
        if (!staticElementsDrawn)
        {
            tft.fillRect(4, 60, 232, 215, BG_COLOR);

            // Main connection info box (full width)
            drawFrame(15, 80, 210, 70, SUCCESS_COLOR, 1);

            // Draw BLE icon
            tft.drawBitmap(25, 90, iconBleConnected, 14, 15, SUCCESS_COLOR, BG_COLOR);

            // Display connection info and module name
            tft.setTextSize(2);
            tft.setFont();
            tft.setTextColor(TEXT_COLOR);
            int nameX = (210 - (strlen(connectedModuleName) * 12)) / 2 + 15;
            tft.setCursor(nameX, 95);
            tft.print(connectedModuleName);

            // Two boxes side by side for stats
            // Left box - Key Presses
            drawFrame(15, 160, 100, 60, TEXT_COLOR, 0);
            tft.setTextSize(1);

            // Center "Key Presses" text
            tft.setTextSize(1);
            tft.getTextBounds(ui_key_presses, 0, 0, &x1, &y1, &w, &h);
            textX = 15 + (100 - w) / 2;
            tft.setCursor(textX, 175);
            tft.setTextColor(TEXT_COLOR);
            tft.print(ui_key_presses);

            // Right box - Signal Strength
            drawFrame(125, 160, 100, 60, TEXT_COLOR, 0);

            // Center "Signal" text
            tft.getTextBounds(ui_signal, 0, 0, &x1, &y1, &w, &h);
            textX = 125 + (100 - w) / 2;
            tft.setCursor(textX, 175);
            tft.setTextColor(TEXT_COLOR);
            tft.print(ui_signal);

            // Draw help text
            tft.setTextSize(1);
            tft.setTextColor(MUTED_COLOR);
            tft.setCursor(15, 250);
            tft.print(ui_long_press_quit);

            // Draw footer
            drawFooter();

            staticElementsDrawn = true;
        }

        // Dynamic content - update only when values change or time passes
        if (shouldUpdateDynamicContent && currentStats)
        {
            // Clear only the dynamic text areas (inset by 15% from each side)
            tft.fillRect(45, 120, 150, 25, BG_COLOR); // Connection time and delay text area
            tft.fillRect(30, 195, 70, 15, BG_COLOR);  // Key presses value area (inside left box)
            tft.fillRect(140, 195, 70, 15, BG_COLOR); // RSSI value area (inside right box)
            tft.fillRect(45, 235, 150, 15, BG_COLOR); // Connection status area

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

            // Center the connection time text (optimized without String concatenation)
            tft.getTextBounds(ui_connected, 0, 0, &x1, &y1, &w, &h);
            uint16_t w1 = w;
            tft.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
            uint16_t w2 = w;
            int totalWidth = w1 + w2;
            textX = 15 + (210 - totalWidth) / 2;

            tft.setCursor(textX, 120);
            tft.setTextColor(TEXT_COLOR);
            tft.print(ui_connected);
            tft.setTextColor(HIGHLIGHT_COLOR);
            tft.print(timeStr);

            // Display estimated delay based on signal strength (optimized without String)
            int estimatedDelay = calculateDelayFromRSSI(currentStats->rssi);
            char delayBuffer[32]; // Buffer for delay text
            uint16_t delayColor;

            if (estimatedDelay < 10)
            {
                sprintf(delayBuffer, "%s%d%s", ui_delay, estimatedDelay, ui_delay_great);
                delayColor = HIGHLIGHT_COLOR;
            }
            else if (estimatedDelay < 20)
            {
                sprintf(delayBuffer, "%s%d%s", ui_delay, estimatedDelay, ui_delay_good);
                delayColor = SUCCESS_COLOR;
            }
            else if (estimatedDelay < 50)
            {
                sprintf(delayBuffer, "%s%d%s", ui_delay, estimatedDelay, ui_delay_ok);
                delayColor = MUTED_COLOR;
            }
            else
            {
                sprintf(delayBuffer, "%s%d%s", ui_delay, estimatedDelay, ui_delay_poor);
                delayColor = ERROR_COLOR;
            }

            // Draw the delay estimation text right below the connection time
            uint16_t w, h;
            tft.getTextBounds(delayBuffer, 0, 0, &x1, &y1, &w, &h);
            textX = 15 + (210 - w) / 2;
            tft.setCursor(textX, 130);
            tft.setTextColor(delayColor);
            tft.print(delayBuffer);

            // Center the key press number (optimized without String)
            char keypressBuffer[16];
            sprintf(keypressBuffer, "%lu", currentStats->keyPresses);
            tft.getTextBounds(keypressBuffer, 0, 0, &x1, &y1, &w, &h);
            textX = 15 + (100 - w) / 2;
            tft.setCursor(textX, 195);
            tft.setTextColor(HIGHLIGHT_COLOR);
            tft.print(keypressBuffer);

            // Center the RSSI value (optimized without String)
            char rssiBuffer[16];
            sprintf(rssiBuffer, "%d dBm", currentStats->rssi);
            tft.getTextBounds(rssiBuffer, 0, 0, &x1, &y1, &w, &h);
            textX = 125 + (100 - w) / 2;
            tft.setCursor(textX, 195);
            tft.setTextColor(HIGHLIGHT_COLOR);
            tft.print(rssiBuffer);

            // Connection status
            tft.getTextBounds(ui_status_connected, 0, 0, &x1, &y1, &w, &h);
            textX = 15 + (210 - w) / 2;
            tft.setCursor(textX, 235);
            tft.setTextColor(SUCCESS_COLOR);
            tft.print(ui_status_connected);
        }
    }
}

/**
 * Calculates an estimated key click delay based on Bluetooth RSSI values
 * Uses fixed-point arithmetic for better performance
 *
 * @param rssi Signal strength in dBm (typically between -100 and -30)
 * @return Estimated delay in milliseconds
 */
int calculateDelayFromRSSI(int rssi)
{
    // RSSI ranges and their corresponding approximate delays
    // Excellent: -30 to -67 dBm: ~5-10ms
    // Good: -68 to -80 dBm: ~10-20ms
    // Fair: -81 to -90 dBm: ~20-50ms
    // Poor: -91 to -100 dBm: ~50-100ms

    // Normalize RSSI to a positive value between 0-70
    // where -30 dBm = 70 (best) and -100 dBm = 0 (worst)
    int normalizedRSSI = constrain(rssi + 100, 0, 70);

    // Map the normalized value to a delay estimate using fixed-point
    // 0 (worst) -> 100ms delay
    // 70 (best) -> 5ms delay
    // delay = 100 - ((normalizedRSSI * 95) / 70)
    int delay = 100 - ((normalizedRSSI * 95) / 70);

    // Add a slight random factor (+/- 10%) using fixed-point
    // random(21) gives 0-20, subtract 10 to get -10 to +10
    int randomFactor = random(21) - 10;
    delay = delay + (delay * randomFactor) / 100;

    return delay;
}
