#include <Adafruit_ILI9341.h>
#include "UI/main.h"
#include "globals.h"
#include "displayHandler.h"

int settingsSelectedOption = 0;
bool inSettingsSubmenu = false;

#define LCD_CS 13
#define LCD_RST 17
#define LCD_RS 16
#define SPI_MOSI 12
#define SPI_SCK 18

extern int hours;
extern int minutes;
extern int seconds;
extern int updatedMinutes;

Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_RS, LCD_RST);

ScreenType currentScreen = MainScreen;

void displayTime(void *parameters);
void clearTime(void *parameters);
void displayTopBar(void *parameters);
void clearTopBar(void *parameters);
void displayDemo(void *parameters);

void displayHandler(void *parameters)
{
    SPI.begin(SPI_SCK, -1, SPI_MOSI);
    tft.begin();
    tft.invertDisplay(true);
    tft.fillScreen(0x10A2);
    tft.setRotation(2);
    Serial.println("Display Handler started.");

    bool capsLockStatus = true;

    bool refreshFlags[3] = {false, false, false};
    switchScreen(MainScreen); // Initialize main screen

    for (;;)
    {
        switch (currentScreen)
        {
        case MainScreen:
            displayTopBar(parameters);
            if (updatedMinutes)
            {
                displayTime(parameters);
                updatedMinutes = false;
            }
            break;

        case SettingsScreen:
            // Screen is updated through knob interactions
            break;

            // Add cases for submenus here

        default:
            break;
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
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

    tft.setTextColor(capsLockStatus ? 0xFD40 : 0x7BEF);
    tft.fillRect(206, 18, 17, 8, 0x10A2);
    tft.setCursor(206, 18);
    tft.print(capsLockStatus ? "ON" : "OFF");
}

void clearTime(void *parameters)
{
    tft.fillRect(75, 51, 90, 130, 0x10A2);
}

void switchScreen(ScreenType newScreen)
{
    if (newScreen == SettingsScreen)
    {
        settingsSelectedOption = 0; // Reset selection when entering settings
        inSettingsSubmenu = false;
    }
    currentScreen = newScreen;
    tft.fillScreen(0x10A2);

    switch (currentScreen)
    {
    case MainScreen:
        displayMainScreen(nullptr);
        break;
    case SettingsScreen:
        displaySettingsScreen(nullptr);
        break;
        // Add cases for submenus here
    }
}

void displayMainScreen(void *parameters)
{
    displayTopBar(parameters);
    displayTime(parameters);
    displayDemo(parameters);
}

void displaySettingsScreen(void *parameters)
{
    const char *menuItems[] = {"Modules", "Keybinds", "Integrations", "RGB"};

    tft.fillScreen(0x10A2);

    // Draw menu items
    tft.setTextSize(2);
    tft.setFont(&FreeSansBold9pt7b);
    for (int i = 0; i < 4; i++)
    {
        tft.setCursor(30, 60 + (i * 30));
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
    tft.setCursor(50, 220);
    tft.print("Powered by PsychOS");
    tft.setCursor(70, 235);
    tft.print("build 0.1.4α");
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