#include <Adafruit_ILI9341.h>
#include "displayContent.h"
#include "globals.h"
#include "icons.h"

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
}

void displayTopBar(void *parameters)
{
    tft.drawBitmap(11, 9, image_menu_settings_sliders_bits, 14, 16, 0xDED9);
    tft.drawBitmap(157, 9, image_cloud_sync_bits, 17, 16, connectionStatus ? 0x9C1F : 0xF22B);
    tft.drawBitmap(182, 9, image_bluetooth_connected_bits, 14, 16, moduleConnectionStatus ? 0x9C1F : 0xF22B);

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
    tft.fillRect(75, 51, 90, 130, 0x10A2);
}

void displayWPM(void *parameters)
{
    clearWPM(parameters);
    tft.setTextColor(0xDED9);
    tft.setTextSize(1);
    tft.setFont();
    tft.setCursor(11, 304);
    tft.print("WPM:" + String(WPMCounter::wpm));
}

void clearWPM(void *parameters)
{
    tft.fillRect(11, 304, 50, 16, 0x10A2);
}

void displayDemo(void *parameters)
{
    tft.setTextSize(1);
    tft.setFont(&FreeMonoBold12pt7b);
    tft.setCursor(18, 250);
    tft.print("Now Playing");

    tft.setFont(&FreeMonoBold9pt7b);
    tft.setCursor(18, 274);
    tft.print("Bury you - Ari.");

    tft.drawBitmap(198, 263, image_music_play_bits, 15, 16, 0xDED9);

    tft.setFont();
    tft.setTextColor(0xFD40);
    tft.setCursor(90, 304);
    tft.print("3D Modeling");
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

    const char *menuItems[] = {"Modules", "Lighting", "Clock", "Iot"};

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
    tft.setTextColor(0x7BEF);
    tft.setCursor(65, 296);
    tft.print("Powered by PsychOS");
    tft.setCursor(79, 305);
    tft.print("build 0.1.4a");
}

void displayRGBSubmenu(void *parameters)
{
    static bool firstDraw = true;
    const char *rgbOptions[] = {"Red", "Green", "Blue", "Brightness"};

    if (firstDraw)
    {
        tft.fillScreen(0x10A2);
        tft.setTextSize(1);
        tft.setFont(&FreeSansBold9pt7b);

        // Draw permanent labels
        for (int i = 0; i < 4; i++)
        {
            tft.setCursor(30, 60 + (i * 30));
            tft.setTextColor(0xDED9, 0x10A2); // Text color, background color
            tft.print(rgbOptions[i]);
        }
        firstDraw = false;
    }

    // Update only dynamic elements
    for (int i = 0; i < 4; i++)
    {
        // Selection highlight
        tft.fillRect(25, 45 + (i * 30), 170, 25,
                     (i == rgbState.currentSelection) ? 0x2945 : 0x10A2);

        // Value display
        tft.setCursor(150, 60 + (i * 30));
        tft.setTextColor((i == rgbState.currentSelection) ? 0xFD40 : 0xDED9,
                         (i == rgbState.currentSelection) ? 0x2945 : 0x10A2);
        tft.print(rgbState.values[i]);
        tft.print(i == 3 ? "%" : "   "); // Add spaces to clear previous values
    }
}

void displayClockSubmenu(void *parameters)
{
    tft.fillScreen(0x10A2); // Clear the screen with a background color

    tft.setTextSize(2);
    tft.setTextColor(0xFFFF); // White color
    tft.setCursor(10, 30);
    tft.print("Set Time");

    tft.setTextSize(3);
    tft.setCursor(10, 70);
    tft.print("Hours: ");
    tft.print(hours);

    tft.setCursor(10, 110);
    tft.print("Minutes: ");
    tft.print(minutes);

    tft.setCursor(10, 150);
    tft.print("Seconds: ");
    tft.print(seconds);

    // Highlight the selected option
    tft.drawRect(5, 65 + settingsSelectedOption * 40, 230, 40, 0xFD40);
}