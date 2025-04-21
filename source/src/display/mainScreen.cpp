#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include "display/displayContent.h"
#include "globals.h"
#include "display/icons.h"
#include "main.h"
#include "translations.h"
#include "display/components/uiComponents.h"
#include "tasks/wpmCounter.h"

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

    tft.drawBitmap(moduleConnectionStatus ? 35 : 36, 9, moduleConnectionStatus ? iconBleConnected : iconBleDisconnected, 14, 15, iconColor, BG_COLOR);
    if (!moduleConnectionStatus)
    {
        tft.fillRect(35, 9, 1, 15, BG_COLOR);
    }

    // Draw the connection status icon (separate from BLE)
    tft.drawBitmap(11, 9, connectionStatus ? (toggleDiscIcon ? iconDisc1 : iconDisc2) : iconDiscMuted, 16, 15, connectionStatus ? SUCCESS_COLOR : ERROR_COLOR, BG_COLOR);

    tft.setTextSize(1);
    tft.setTextColor(TEXT_COLOR);
    tft.setFont();
    tft.setCursor(206, 8);
    tft.print(ui_caps);

    // Add a static variable to track the previous capsLockStatus
    static bool previousCapsLockStatus = !capsLockStatus; // Initialize to a different value to ensure the first update

    // Check if capsLockStatus has changed
    if (capsLockStatus != previousCapsLockStatus)
    {
        tft.drawBitmap(206, 18, capsLockStatus ? textOff : textOn, capsLockStatus ? 14 : 9, 5, capsLockStatus ? BG_COLOR : BG_COLOR, BG_COLOR);
        previousCapsLockStatus = capsLockStatus; // Update the previous status
    }

    tft.drawBitmap(206, 18, capsLockStatus ? textOn : textOff, capsLockStatus ? 9 : 14, 5, capsLockStatus ? HIGHLIGHT_COLOR : MUTED_COLOR, BG_COLOR);
}

void displayTime(void *parameters)
{
    clearTime(parameters);
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(8);
    tft.setFont();

    char timeString[3];
    sprintf(timeString, "%02d", hours);
    tft.setCursor(75, 60);
    tft.print(timeString);

    sprintf(timeString, "%02d", minutes);
    tft.setCursor(75, 122);
    tft.print(timeString);
}

void displayWPM(void *parameters)
{
    clearWPM(parameters);
    tft.setTextColor(TEXT_COLOR);
    tft.setTextSize(2);
    tft.setFont();
    tft.setCursor(11, 297);
    tft.print(String(WPMCounter::wpm));

    // Adjust the position of "WPM" based on the number of digits in WPM
    int wpmLength = String(WPMCounter::wpm).length();
    int wpmOffset = 24 + (wpmLength - 1) * 12; // Adjust offset dynamically

    tft.setTextColor(MUTED_COLOR);
    tft.setTextSize(1);
    tft.setCursor(wpmOffset, 304);
    tft.print(ui_wpm);
}

void displayLAYER(void *parameters)
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
    tft.print(ui_song_title);

    tft.drawBitmap(200, 241, iconMusicPlay, 20, 18, TEXT_COLOR);
}