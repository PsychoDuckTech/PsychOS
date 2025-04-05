#include <Arduino.h>
#include "tasks/displayHandler.h"
#include "display/screens.h"
#include "globals.h"
#include "main.h"

int firstDraw = 1;
QueueHandle_t settingsRotationQueue = NULL;

extern bool updatedMinutes;

// Declare global variables
Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_RS, LCD_RST);
ScreenType currentScreen = MainScreen;
int settingsSelectedOption = 0;
bool inSettingsSubmenu = false;
bool updateMainScreen = true;
SemaphoreHandle_t screenMutex;

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
        // First sync with actual RGB task values before setting needsRefresh
        uRGB.syncUIValues();
        // Add a small delay to ensure the RGB task has updated rgbState
        vTaskDelay(pdMS_TO_TICKS(10));
        needsFullRedraw = true; // Force full redraw of screen
        rgbState.needsRefresh = true; // Force full redraw of RGB content
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

    screenMutex = xSemaphoreCreateMutex();
    settingsRotationQueue = xQueueCreate(10, sizeof(SettingsRotationEvent));
    switchScreen(MainScreen);

    for (;;)
    {
        xSemaphoreTake(screenMutex, portMAX_DELAY);

        // Process any pending rotation events for settings and clock screens
        if (currentScreen == SettingsScreen || currentScreen == ClockSubmenu)
        {
            SettingsRotationEvent event;
            int totalRotation = 0;

            // Collect all pending rotation events
            while (xQueueReceive(settingsRotationQueue, &event, 0) == pdTRUE)
            {
                totalRotation += event.totalSteps;
            }

            // Apply accumulated rotation if any
            if (totalRotation != 0)
            {
                if (currentScreen == SettingsScreen)
                {
                    // Calculate new position considering total rotation
                    int newPosition = settingsSelectedOption - totalRotation;

                    // Clamp the position between 0 and 3 without wrapping
                    if (newPosition < 0)
                        newPosition = 0;
                    if (newPosition > 3)
                        newPosition = 3;

                    if (settingsSelectedOption != newPosition)
                    {
                        settingsSelectedOption = newPosition;
                        displaySettingsScreen(nullptr);
                    }
                }
                else if (currentScreen == ClockSubmenu)
                {
                    // Handle clock value updates
                    switch (settingsSelectedOption)
                    {
                    case 0: // Hours
                        hours = (hours - totalRotation + 24) % 24;
                        break;
                    case 1: // Minutes
                        minutes = (minutes - totalRotation + 60) % 60;
                        break;
                    case 2: // Seconds
                        seconds = (seconds - totalRotation + 60) % 60;
                        break;
                    }
                    displayClockSubmenu(nullptr);
                }
            }
        }

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