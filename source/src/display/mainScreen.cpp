#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include <cmath>
#include "display/displayContent.h"
#include "globals.h"
#include "display/icons.h"
#include "main.h"
#include "translations.h"
#include "display/components/uiComponents.h"
#include "tasks/wpmCounter.h"
#include "tasks/mediaHandler.h"

extern Adafruit_ILI9341 tft;
extern bool connectionStatus;
extern bool moduleConnectionStatus;
extern bool capsLockStatus;
extern int hours;
extern int minutes;
extern bool needsFullRedraw;
extern int rgbValues[4];
extern unsigned long lastToggleTime;
extern bool toggleDiscIcon;

void displayMainScreen(void *parameters)
{
    Serial.println("displayMainScreen called");
    needsFullRedraw = true;

    displayTopBar(parameters);
    displayTime(parameters);
    displayWPM(parameters);
    displayLAYER(parameters);
    displayDemo(parameters); // demo is the last thing being loaded as it takes the longest at loading.
}

void displayTopBar(void *parameters)
{
    // Check if 750ms have passed to toggle the icon
    if (connectionStatus && millis() - lastToggleTime >= 750)
    {
        toggleDiscIcon = !toggleDiscIcon;
        lastToggleTime = millis();
    }

    // Get current time to check for recent data reception
    unsigned long currentTime = millis();
    uint16_t iconColor;

    if (moduleConnectionStatus)
    {
        // If data was received in the last 100ms, show highlight color
        if (currentTime - lastDataReceivedTime < 65)
        {
            iconColor = HIGHLIGHT_COLOR; // Flash with highlight color for data reception
        }
        else
        {
            iconColor = SUCCESS_COLOR; // Normal connected color
        }
    }
    else
    {
        iconColor = MUTED_COLOR; // Disconnected color
    }

    // Track previous BLE state to avoid unnecessary redraws
    static bool prevModuleConnectionStatus = !moduleConnectionStatus;
    static uint16_t prevIconColor = 0xFFFF; // Invalid color to force first draw

    // Force redraw on full screen refresh
    bool forceRedraw = needsFullRedraw;

    // Only redraw BLE icon if status or color changed, or if forcing redraw
    if (forceRedraw || moduleConnectionStatus != prevModuleConnectionStatus || iconColor != prevIconColor)
    {
        tft.drawBitmap(moduleConnectionStatus ? 35 : 36, 9, moduleConnectionStatus ? iconBleConnected : iconBleDisconnected, 14, 15, iconColor, BG_COLOR);
        if (!moduleConnectionStatus)
        {
            tft.fillRect(35, 9, 1, 15, BG_COLOR);
        }
        prevModuleConnectionStatus = moduleConnectionStatus;
        prevIconColor = iconColor;
    }

    // Draw the connection status icon (separate from BLE)
    // Track previous connection icon state to avoid unnecessary redraws
    static bool prevConnectionStatus = !connectionStatus;
    static bool prevToggleDiscIcon = !toggleDiscIcon;
    
    // Force redraw on full screen refresh
    bool forceConnectionRedraw = needsFullRedraw;
    
    // Only redraw connection icon if status changed or icon toggled, or forcing redraw
    if (forceConnectionRedraw || connectionStatus != prevConnectionStatus || (connectionStatus && toggleDiscIcon != prevToggleDiscIcon))
    {
        tft.drawBitmap(11, 9, connectionStatus ? (toggleDiscIcon ? iconDisc1 : iconDisc2) : iconDiscMuted, 16, 15, connectionStatus ? SUCCESS_COLOR : ERROR_COLOR, BG_COLOR);
        prevConnectionStatus = connectionStatus;
        prevToggleDiscIcon = toggleDiscIcon;
    }

    // Draw the static "CAPS" text only on full screen refresh
    if (needsFullRedraw)
    {
        tft.setTextSize(1);
        tft.setTextColor(TEXT_COLOR);
        tft.setFont();
        tft.setCursor(206, 8);
        tft.print(ui_caps);
    }

    // Add a static variable to track the previous capsLockStatus
    static bool previousCapsLockStatus = !capsLockStatus; // Initialize to a different value to ensure the first update

    // Force redraw on full screen refresh
    bool forceCapsRedraw = needsFullRedraw;

    // Check if capsLockStatus has changed
    if (forceCapsRedraw || capsLockStatus != previousCapsLockStatus)
    {
        // Clear the caps lock status area
        tft.fillRect(206, 18, 14, 5, BG_COLOR);
        // Draw the correct status text
        tft.drawBitmap(206, 18, capsLockStatus ? textOn : textOff, capsLockStatus ? 9 : 14, 5, capsLockStatus ? HIGHLIGHT_COLOR : MUTED_COLOR, BG_COLOR);
        previousCapsLockStatus = capsLockStatus; // Update the previous status
    }
}

void displayTime(void *parameters)
{
    // Track previous time values to avoid unnecessary redraws
    static int previousHours = -1;
    static int previousMinutes = -1;
    
    int currentHours = hours;
    int currentMinutes = minutes;
    
    // Force redraw on full screen refresh
    bool forceRedraw = needsFullRedraw;
    
    // Only redraw if time changed or forcing redraw
    if (forceRedraw || currentHours != previousHours || currentMinutes != previousMinutes)
    {
        clearTime(parameters);
        tft.setTextColor(TEXT_COLOR);
        tft.setTextSize(8);
        tft.setFont();

        char timeString[3];
        sprintf(timeString, "%02d", currentHours);
        tft.setCursor(75, 60);
        tft.print(timeString);

        sprintf(timeString, "%02d", currentMinutes);
        tft.setCursor(75, 122);
        tft.print(timeString);
        
        previousHours = currentHours;
        previousMinutes = currentMinutes;
    }
}

void displayWPM(void *parameters)
{
    // Track previous WPM to avoid unnecessary redraws
    static int previousWPM = -1;
    static int previousDigits = -1;
    
    int currentWPM = WPMCounter::wpm;
    
    // Calculate digits without String allocation or expensive log10()
    int absWPM = abs(currentWPM);
    int currentDigits = 1; // At least 1 digit
    if (absWPM >= 10) currentDigits = 2;
    if (absWPM >= 100) currentDigits = 3;
    if (absWPM >= 1000) currentDigits = 4; // Though unlikely for WPM
    if (currentWPM < 0) currentDigits++; // Account for negative sign
    
    // Force redraw on full screen refresh
    bool forceRedraw = needsFullRedraw;
    
    // Only redraw if WPM changed or forcing redraw
    if (forceRedraw || currentWPM != previousWPM || currentDigits != previousDigits)
    {
        clearWPM(parameters);
        tft.setTextColor(TEXT_COLOR);
        tft.setTextSize(2);
        tft.setFont();
        tft.setCursor(11, 297);
        
        // Print WPM directly without String allocation
        char wpmStr[8];
        sprintf(wpmStr, "%d", currentWPM);
        tft.print(wpmStr);

        // Adjust the position of "WPM" based on the number of digits in WPM
        int wpmOffset = 24 + (currentDigits - 1) * 12; // Adjust offset dynamically

        tft.setTextColor(MUTED_COLOR);
        tft.setTextSize(1);
        tft.setCursor(wpmOffset, 304);
        tft.print(ui_wpm);
        
        previousWPM = currentWPM;
        previousDigits = currentDigits;
    }
}

void displayLAYER(void *parameters)
{
    // Only draw static layer info on full screen refresh
    if (needsFullRedraw)
    {
        tft.setTextColor(MUTED_COLOR);
        tft.setTextSize(1);
        tft.setFont();
        tft.setCursor(186, 304);
        tft.print(ui_layer);
        tft.setTextColor(HIGHLIGHT_COLOR);
        tft.setTextSize(2);
        tft.setCursor(219, 297);
        tft.print("3");
    }
}

void displayDemo(void *parameters)
{
    tft.drawBitmap(9, 221, MusicPlayerBorder, 222, 58, TEXT_COLOR);
    tft.drawBitmap(13, 262, MusicPlayerShadow1, 214, 17, 0x18C3);
    tft.drawBitmap(13, 252, MusicPlayerShadow2, 214, 11, 0x861);

    tft.setTextSize(2);
    tft.setTextColor(TEXT_COLOR);
    tft.setFont();
    tft.setCursor(21, 233);
    tft.print(ui_now_playing);

    tft.setCursor(21, 253);
    tft.print(currentSongTitle); // Use the dynamic song title instead of the static translation

    tft.drawBitmap(200, 241, iconMusicPlay, 20, 18, TEXT_COLOR);
}