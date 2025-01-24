#include <Adafruit_ILI9341.h>
#include "displayHandler.h"
#include "display/displayContent.cpp"
#include "globals.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

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
bool updateMainScreen = true; // Add this flag
extern int updatedMinutes;
SemaphoreHandle_t screenMutex; // Add a mutex

void switchScreen(ScreenType newScreen)
{
    xSemaphoreTake(screenMutex, portMAX_DELAY); // Take the mutex before switching screens

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
        updateMainScreen = true; // Set flag to true when switching to main screen
        displayMainScreen(nullptr);
        break;
    case SettingsScreen:
        updateMainScreen = false; // Set flag to false when switching away from main screen
        displaySettingsScreen(nullptr);
        break;
        // Add cases for submenus here
    }

    xSemaphoreGive(screenMutex); // Release the mutex after switching screens
}

void displayHandler(void *parameters)
{
    SPI.begin(SPI_SCK, -1, SPI_MOSI);
    tft.begin();
    tft.invertDisplay(true);
    tft.fillScreen(0x10A2);
    tft.setRotation(2);
    Serial.println("Display Handler started.");

    screenMutex = xSemaphoreCreateMutex(); // Create the mutex
    switchScreen(MainScreen);              // Initialize main screen

    for (;;)
    {
        xSemaphoreTake(screenMutex, portMAX_DELAY); // Take the mutex before updating the screen

        switch (currentScreen)
        {
        case MainScreen:
            if (updateMainScreen) // Check the flag before updating the main screen
            {
                displayTopBar(parameters);
                if (updatedMinutes)
                {
                    displayTime(parameters);
                    updatedMinutes = false;
                }
            }
            break;

        case SettingsScreen:
            // Screen is updated through knob interactions
            break;

            // Add cases for submenus here

        default:
            break;
        }

        xSemaphoreGive(screenMutex); // Release the mutex after updating the screen
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}