#include <Arduino.h>
// #include <Adafruit_ILI9341.h>
#include "displayHandler.h"
#include "display/displayContent.cpp"
#include "globals.h"

// Define constants
#define LCD_CS 13
#define LCD_RST 17
#define LCD_RS 16
#define SPI_MOSI 12
#define SPI_SCK 18

int firstDraw = 1;

RGBState rgbState = {
    .currentSelection = 0,
    .values = {255, 255, 255, 100}, // Default values
    .needsRefresh = true};

extern bool updatedMinutes; // Declaration of external variable

// Declare global variables
Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_RS, LCD_RST);
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
    case RGBSubmenu:
        updateMainScreen = false;
        rgbState.needsRefresh = true; // Force full redraw
        break;
    case ModulesSubmenu:
        updateMainScreen = false;
        displayModulesSubmenu(nullptr);
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

        case RGBSubmenu:
            displayRGBSubmenu(parameters);
            break;

            // Add cases for submenus here

        default:
            break;
        }

        xSemaphoreGive(screenMutex); // Release the mutex after updating the screen
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void startDisplayTask(UBaseType_t core = 1, uint32_t stackDepth = 4096, UBaseType_t priority = 1)
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

void handleRotation(int rotation)
{
    if (currentScreen == SettingsScreen)
    {
        settingsSelectedOption = (settingsSelectedOption + (rotation > 0 ? 1 : -1)) % 4;
        if (settingsSelectedOption < 0)
            settingsSelectedOption = 3;
        displaySettingsScreen(nullptr); // Refresh display
    }
    else if (currentScreen == RGBSubmenu)
    {
        int8_t delta = rotation > 0 ? 1 : -1;
        if (rgbState.currentSelection == 3)
        { // Brightness
            rgbState.values[3] = constrain(rgbState.values[3] + delta, 0, 100);
        }
        else
        { // RGB channels
            rgbState.values[rgbState.currentSelection] =
                constrain(rgbState.values[rgbState.currentSelection] + delta, 0, 255);
        }
        rgbState.needsRefresh = true;
        displayRGBSubmenu(nullptr); // Refresh display
    }
    else if (currentScreen == ModulesSubmenu)
    {
        settingsSelectedOption = (settingsSelectedOption + (rotation > 0 ? 1 : -1)) % moduleCount;
        if (settingsSelectedOption < 0)
            settingsSelectedOption = moduleCount - 1;
        displayModulesSubmenu(nullptr); // Refresh display
    }
    else if (currentScreen == ClockSubmenu)
    {
        int8_t delta = rotation > 0 ? 1 : -1;
        switch (settingsSelectedOption)
        {
        case 0:
            hours = (hours + delta + 24) % 24;
            break;
        case 1:
            minutes = (minutes + delta + 60) % 60;
            break;
        case 2:
            seconds = (seconds + delta + 60) % 60;
            break;
        }
        displayClockSubmenu(nullptr); // Refresh display
    }
}

void handleShortPress()
{
    if (currentScreen == SettingsScreen)
    {
        switch (settingsSelectedOption)
        {
        case 0:
            switchScreen(ModulesSubmenu);
            break;
        case 1:
            switchScreen(KeybindsSubmenu);
            break;
        case 2:
            switchScreen(IntegrationsSubmenu);
            break;
        case 3:
            switchScreen(ClockSubmenu);
            break;
        }
    }
    else if (currentScreen == RGBSubmenu)
    {
        rgbState.currentSelection = (rgbState.currentSelection + 1) % 4;
        rgbState.needsRefresh = true;
        displayRGBSubmenu(nullptr); // Refresh display
    }
    else if (currentScreen == ClockSubmenu)
    {
        settingsSelectedOption = (settingsSelectedOption + 1) % 3;
        displayClockSubmenu(nullptr); // Refresh display
    }
}

void handleLongPress()
{
    if (currentScreen == MainScreen)
    {
        switchScreen(SettingsScreen);
    }
    else
    {
        switchScreen(MainScreen);
    }
}

void handleDoublePress()
{
    if (currentScreen == MainScreen)
    {
        capsLockStatus = !capsLockStatus;
    }
}