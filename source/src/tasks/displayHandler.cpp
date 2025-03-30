#include <Arduino.h>
// #include <Adafruit_ILI9341.h>
#include "tasks/displayHandler.h"
#include "display/screens.h"
#include "globals.h"
#include "main.h" // Add main.h include to get pin definitions

int firstDraw = 1;

extern bool updatedMinutes; // Declaration of external variable

// Declare global variables
Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_RS, LCD_RST); // Use the pins defined in main.h
ScreenType currentScreen = MainScreen;
int settingsSelectedOption = 0;
bool inSettingsSubmenu = false;
bool updateMainScreen = true;  // Add this flag
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
    tft.fillScreen(BG_COLOR);

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
    case RGBLightingSubmenu:
        updateMainScreen = false;
        rgbState.needsRefresh = true; // Force full redraw
        break;
    case ClockSubmenu:
        updateMainScreen = false;
        displayClockSubmenu(nullptr);
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
    tft.fillScreen(BG_COLOR);
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
                if (WPMCounter::updated)
                {
                    displayWPM(parameters);
                    WPMCounter::updated = false;
                }
            }
            break;

        case SettingsScreen:
            // Screen is updated through knob interactions
            break;

        case RGBLightingSubmenu:
            displayRGBSubmenu(parameters);
            break;

            // Add cases for submenus here

        default:
            break;
        }

        xSemaphoreGive(screenMutex); // Release the mutex after updating the screen
        vTaskDelay(25 / portTICK_PERIOD_MS);
    }
}

void startDisplayTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t displayHandle;
    xTaskCreatePinnedToCore(
        displayHandler,    // Function to be called
        "Display Handler", // Name of the task
        stackDepth,        // Stack size in words
        NULL,              // Task input parameter
        priority,          // Priority of the task
        &displayHandle,    // Task handle
        core               // Core where the task should run
    );
}