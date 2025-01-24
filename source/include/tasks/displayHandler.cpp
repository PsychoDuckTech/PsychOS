#include <Adafruit_ILI9341.h>
#include "displayHandler.h"
#include "display/displayContent.cpp"
#include "globals.h"

// Define constants
#define LCD_CS 13
#define LCD_RST 17
#define LCD_RS 16
#define SPI_MOSI 12
#define SPI_SCK 18

// Declare global variables
Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_RS, LCD_RST);
ScreenType currentScreen = MainScreen;
int settingsSelectedOption = 0;
bool inSettingsSubmenu = false;
extern int updatedMinutes;

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

void displayHandler(void *parameters)
{
    SPI.begin(SPI_SCK, -1, SPI_MOSI);
    tft.begin();
    tft.invertDisplay(true);
    tft.fillScreen(0x10A2);
    tft.setRotation(2);
    Serial.println("Display Handler started.");

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