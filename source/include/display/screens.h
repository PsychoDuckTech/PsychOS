#include <Adafruit_ILI9341.h>
#include "displayContent.h"
#include "globals.h"
#include "icons.h"
#include "main.h"

extern Adafruit_ILI9341 tft;
extern bool connectionStatus;
extern bool moduleConnectionStatus;
extern bool capsLockStatus;
extern int hours;
extern int minutes;

int rgbValues[4] = {255, 255, 255, 100};

void displayMainScreen(void *parameters)
{
    displayTopBar(parameters);
    displayTime(parameters);
    displayWPM(parameters);
    displayDemo(parameters);
    displayLAYER(parameters);
}

void displayTopBar(void *parameters)
{
    //.drawBitmap(11, 9, image_menu_settings_sliders_bits, 14, 16, 0xDED9);
    // tft.drawBitmap(157, 9, image_cloud_sync_bits, 17, 16, connectionStatus ? 0x9C1F, BG_COLOR : 0xF22B, BG_COLOR);
    // tft.drawBitmap(182, 9, image_bluetooth_connected_bits, 14, 16, moduleConnectionStatus ? 0x9C1F : 0xF22B);
    tft.drawBitmap(11, 9, connectionStatus ? iconDisc : iconDiscMuted, 16, 15, connectionStatus ? SUCCESS_COLOR : ERROR_COLOR, BG_COLOR);
    // tft.drawBitmap(36, 9, iconBleConnected, 14, 16, moduleConnectionStatus ? SUCCESS_COLOR : BG_COLOR);
    tft.drawBitmap(36, 9, moduleConnectionStatus ? iconBleConnected : iconBleDisconnected, 14, 15, moduleConnectionStatus ? SUCCESS_COLOR : MUTED_COLOR, BG_COLOR);

    tft.setTextSize(1);
    tft.setTextColor(TEXT_COLOR);
    tft.setFont();
    tft.setCursor(206, 8);
    tft.print("Caps");

    tft.drawBitmap(206, 18, capsLockStatus ? textOff : textOn, capsLockStatus ? 14 : 9, 5, capsLockStatus ? BG_COLOR : BG_COLOR, BG_COLOR);
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

void clearTime(void *parameters)
{
    tft.fillRect(75, 51, 90, 130, BG_COLOR);
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
    tft.print("WPM");
}

void clearWPM(void *parameters)
{
    tft.fillRect(11, 297, 55, 16, BG_COLOR);
}

void displayLAYER(void *parameters)
{
    tft.setTextColor(MUTED_COLOR);
    tft.setTextSize(1);
    tft.setFont();
    tft.setCursor(186, 304);
    tft.print("LAYER");
    tft.setTextColor(HIGHLIGHT_COLOR);
    tft.setTextSize(2);
    tft.setCursor(219, 297);
    tft.print("3");
}

void clearLAYER(void *parameters)
{
    tft.fillRect(11, 297, 50, 16, BG_COLOR);
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
    tft.print("Now Playing");

    tft.setCursor(21, 253);
    tft.print("Bury you");

    tft.drawBitmap(200, 241, iconMusicPlay, 20, 18, TEXT_COLOR);
}

void displaySettingsScreen(void *parameters)
{
    tft.setTextSize(2);
    tft.setFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_COLOR);
    // Centered title
    int titleWidth = strlen("Settings") * 16; // Approximate width calculation
    tft.setCursor((tft.width() - titleWidth) / 2, 40);
    tft.print("Settings");

    const char *menuItems[] = {"Modules", "RGB Glow", "Clock", "IoT link"};

    // Draw menu items
    tft.setTextSize(2);
    tft.setFont(&FreeSansBold9pt7b);
    for (int i = 0; i < 4; i++)
    {
        tft.setCursor(11, 85 + (i * 35));
        if (i == settingsSelectedOption)
        {
            tft.setTextColor(HIGHLIGHT_COLOR); // Highlight selected
        }
        else
        {
            tft.setTextColor(TEXT_COLOR); // Normal color
        }
        tft.print(menuItems[i]);
    }

    // Footer text
    tft.setTextSize(1);
    tft.setFont();
    tft.setTextColor(MUTED_COLOR);
    tft.setCursor(65, 296);
    tft.print("Powered by PsychOS");
    tft.setCursor(79, 305);
    tft.print(String("build ") + String(OS_version));
}

void displayRGBSubmenu(void *parameters)
{
    const char *rgbOptions[] = {"Red", "Green", "Blue", "Brightness"};
    const int valueRanges[] = {255, 255, 255, 100};

    if (rgbState.needsRefresh)
    {
        tft.fillScreen(BG_COLOR);

        // Title
        tft.setTextSize(2);
        tft.setFont(&FreeSansBold9pt7b);
        tft.setTextColor(TEXT_COLOR);
        tft.setCursor(25, 30);
        tft.print("RGB Glow");

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
        tft.print("Rotate: Adjust Value");
        tft.setCursor(15, 258);
        tft.print("Press: Next Parameter");
        tft.setCursor(15, 266);
        tft.print("Long Press: Quit to Menu");

        rgbState.needsRefresh = false;
    }
}

void displayClockSubmenu(void *parameters)
{
    static int lastSelectedOption = -1;
    static int lastValues[3] = {-1, -1, -1}; // Track last values to detect changes

    if (lastSelectedOption == -1)
        tft.fillScreen(BG_COLOR); // Initial draw

    tft.setFont(&FreeSansBold9pt7b);
    tft.setTextSize(2);
    tft.setTextColor(TEXT_COLOR);
    tft.setCursor(40, 30);
    tft.print("Set Time");

    const char *labels[] = {"Hours", "Minutes", "Seconds"};
    int values[] = {hours, minutes, seconds};
    for (int i = 0; i < 3; i++)
    {
        int yPos = 80 + (i * 50);

        // Clear the area of the previously selected option or if the value has changed
        if (i == lastSelectedOption || values[i] != lastValues[i])
        {
            tft.fillRect(10, yPos - 15, 220, 40, BG_COLOR);
        }

        if (i == settingsSelectedOption)
        {
            tft.fillRect(10, yPos - 15, 220, 40, SELECTED_COLOR);
        }

        tft.setFont(&FreeSansBold9pt7b);
        tft.setTextSize(1);
        tft.setTextColor(TEXT_COLOR);
        tft.setCursor(20, yPos);
        tft.print(labels[i]);
        tft.setCursor(120, yPos);
        tft.setTextColor(i == settingsSelectedOption ? HIGHLIGHT_COLOR : TEXT_COLOR);
        char valStr[3];
        sprintf(valStr, "%02d", values[i]);
        tft.print(valStr);
    }

    // Hint text
    tft.setFont();
    tft.setTextSize(1);
    tft.setTextColor(MUTED_COLOR);
    tft.setCursor(20, 280);
    tft.print("Rotate: Adjust | Press: Next");

    lastSelectedOption = settingsSelectedOption; // Track for partial updates
    for (int i = 0; i < 3; i++)
    {
        lastValues[i] = values[i]; // Update last values
    }
}

void displayModulesSubmenu(void *parameters)
{
    static bool firstDraw = true;
    const char *noModuleMessage = "No module connected";
    const char *moduleName = "Connected Module"; // Replace with actual module name

    if (firstDraw)
    {
        tft.fillScreen(BG_COLOR);
        tft.setTextSize(2);
        tft.setFont(&FreeSansBold9pt7b);
        tft.setTextColor(TEXT_COLOR);

        // Draw title
        int titleWidth = strlen("Modules") * 16; // Approximate width calculation
        tft.setCursor((tft.width() - titleWidth) / 2, 40);
        tft.print("Modules");

        firstDraw = false;
    }

    // Clear previous module name or message
    tft.fillRect(11, 85, 200, 35, BG_COLOR);

    // Display message or module name
    tft.setCursor(11, 85);
    if (!moduleConnectionStatus)
    {
        tft.setTextColor(ERROR_COLOR);
        tft.print(noModuleMessage);
    }
    else
    {
        tft.setTextColor(TEXT_COLOR);
        tft.print(moduleName);
    }

    // Footer text
    tft.setTextSize(1);
    tft.setFont();
    tft.setTextColor(MUTED_COLOR);
    tft.setCursor(65, 296);
    tft.print("Powered by PsychOS");
    tft.setCursor(79, 305);
    tft.print("build 0.1.4a");
}