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
    tft.drawBitmap(11, 9, image_cloud_sync_bits, 17, 16, connectionStatus ? SUCCESS_COLOR, BG_COLOR : ERROR_COLOR, BG_COLOR);
    tft.drawBitmap(36, 9, image_bluetooth_connected_bits, 14, 16, moduleConnectionStatus ? SUCCESS_COLOR : BG_COLOR);

    tft.setTextSize(1);
    tft.setTextColor(0xDED9);
    tft.setFont();
    tft.setCursor(206, 8);
    tft.print("Caps");

    tft.setCursor(206, 18);
    tft.setTextColor(0x10A2);
    tft.print(capsLockStatus ? "OFF" : "ON");
    tft.setCursor(206, 18);
    tft.setTextColor(capsLockStatus ? 0xFD40 : 0x7BEF);
    tft.print(capsLockStatus ? "ON" : "OFF");
}

void displayTime(void *parameters)
{
    clearTime(parameters);
    tft.setTextColor(0xDED9);
    tft.setTextSize(4);
    tft.setFont(&FreeSansBold9pt7b);

    char timeString[3];
    sprintf(timeString, "%02d", hours);
    tft.setCursor(79, 100);
    tft.print(timeString);

    sprintf(timeString, "%02d", minutes);
    tft.setCursor(79, 175);
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
    tft.setTextColor(MUTED_COLOR);
    tft.setTextSize(1);
    tft.setCursor(36, 304);
    tft.print("WPM");
}

void clearWPM(void *parameters)
{
    tft.fillRect(11, 297, 50, 16, BG_COLOR);
}

void displayLAYER(void *parameters)
{
    tft.setTextColor(MUTED_COLOR);
    tft.setTextSize(1);
    tft.setFont();
    tft.setCursor(186, 304);
    tft.print("LAYER");
    tft.setTextColor(TEXT_COLOR);
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
    tft.setTextSize(1);
    tft.setTextColor(TEXT_COLOR);
    tft.setFont(&FreeMonoBold12pt7b);
    tft.setCursor(18, 250);
    tft.print("Now Playing");

    tft.setFont(&FreeMonoBold9pt7b);
    tft.setCursor(18, 274);
    tft.print("Bury you - Ari.");

    tft.drawBitmap(198, 263, image_music_play_bits, 15, 16, 0xDED9);
}

void displaySettingsScreen(void *parameters)
{
    tft.setTextSize(2);
    tft.setFont(&FreeSansBold9pt7b);
    tft.setTextColor(0xDED9);
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
            tft.setTextColor(0xFD40); // Highlight selected
        }
        else
        {
            tft.setTextColor(0xDED9); // Normal color
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
        tft.setTextColor(0xDED9);

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