#include <Arduino.h>
#include "tasks/displayHandler.h"
#include "display/screens.h"
#include "globals.h"
#include "main.h"

int firstDraw = 1;
QueueHandle_t settingsRotationQueue = NULL;

extern bool updatedMinutes;

// Settings menu configuration (mirrored from settingsScreen.cpp)
const int SETTINGS_MENU_ITEM_COUNT = 6;

// Declare global variables
Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_RS, LCD_RST);
ScreenType currentScreen = MainScreen;
int settingsSelectedOption = 0;
bool inSettingsSubmenu = false;
bool updateMainScreen = true;
bool requestMainScreenSwitch = false;
SemaphoreHandle_t screenMutex;

void handleMainScreenSwitch()
{
    updateMainScreen = true;
    Serial.println("Calling displayMainScreen");
    displayMainScreen(nullptr);
}

void handleSettingsScreenSwitch()
{
    updateMainScreen = false;
    displaySettingsScreen(nullptr);
}

void handleModulesSubmenuSwitch()
{
    updateMainScreen = false;
    needsFullRedraw = true;
    displayModulesSubmenu(nullptr);
}

void handleRGBLightingSubmenuSwitch()
{
    updateMainScreen = false;
    uRGB.syncUIValues();
    vTaskDelay(pdMS_TO_TICKS(10));
    needsFullRedraw = true;
    rgbState.needsRefresh = true;
}

void handleClockSubmenuSwitch()
{
    updateMainScreen = false;
    displayClockSubmenu(nullptr);
}

void handlePixelFlushScreenSwitch()
{
    updateMainScreen = false;
    Serial.println("Calling displayPixelFlushScreen");
    displayPixelFlushScreen(nullptr);
    // Create the pixel flush task after screen initialization
    xTaskCreatePinnedToCore(
        startPixelFlush, // Function to be called
        "Pixel Flush",   // Name of task
        4096,            // Stack size
        NULL,            // Task input parameter
        1,               // Priority
        NULL,            // Task handle
        0                // Core where the task should run
    );
}

void switchScreen(ScreenType newScreen)
{
    Serial.print("Switching screen to: ");
    Serial.println(newScreen);
    xSemaphoreTake(screenMutex, portMAX_DELAY);

    if (newScreen == SettingsScreen)
    {
        settingsSelectedOption = 0;
        inSettingsSubmenu = false;
    }
    currentScreen = newScreen;
    tft.fillScreen(BG_COLOR);

    switch (currentScreen)
    {
    case MainScreen:
        handleMainScreenSwitch();
        break;
    case SettingsScreen:
        handleSettingsScreenSwitch();
        break;
    case ModulesSubmenu:
        handleModulesSubmenuSwitch();
        break;
    case RGBLightingSubmenu:
        handleRGBLightingSubmenuSwitch();
        break;
    case ClockSubmenu:
        handleClockSubmenuSwitch();
        break;
    case PixelFlushScreen:
        handlePixelFlushScreenSwitch();
        break;
    }

    xSemaphoreGive(screenMutex); // Release the mutex after switching screens
    Serial.println("Screen switch complete");
}

void displayHandler(void *parameters)
{
    SPI.begin(SPI_SCK, -1, SPI_MOSI);
    tft.begin();
    tft.invertDisplay(true);
    tft.fillScreen(BG_COLOR);
    tft.setRotation(0);
    Serial.println("Display Handler started.");

    screenMutex = xSemaphoreCreateMutex();
    settingsRotationQueue = xQueueCreate(10, sizeof(SettingsRotationEvent));
    switchScreen(MainScreen);

    for (;;)
    {
        xSemaphoreTake(screenMutex, portMAX_DELAY);

        // Check if we need to switch to main screen
        if (requestMainScreenSwitch)
        {
            requestMainScreenSwitch = false;
            xSemaphoreGive(screenMutex);
            if (currentScreen != MainScreen)
            {
                Serial.println("Processing pending switch to MainScreen");
                switchScreen(MainScreen);
            }
            continue;
        }

        // Check pixel flush completion
        if (currentScreen == PixelFlushScreen && pixelFlushComplete)
        {
            Serial.println("Pixel flush complete, requesting MainScreen switch");
            pixelFlushComplete = false;
            updateMainScreen = true;
            requestMainScreenSwitch = true;
            xSemaphoreGive(screenMutex);
            continue;
        }

        // Process any pending rotation events for settings and clock screens
        if (currentScreen == SettingsScreen || currentScreen == ClockSubmenu)
        {
            SettingsRotationEvent event;

            // Process rotation events one by one for immediate response
            if (xQueueReceive(settingsRotationQueue, &event, 0) == pdTRUE)
            {
                if (currentScreen == SettingsScreen)
                {
                    const int MAX_VISIBLE_ITEMS = 4;
                    int currentPage = settingsSelectedOption / MAX_VISIBLE_ITEMS;
                    int itemsOnCurrentPage = min(MAX_VISIBLE_ITEMS, SETTINGS_MENU_ITEM_COUNT - (currentPage * MAX_VISIBLE_ITEMS));
                    int lastItemOnPage = (currentPage * MAX_VISIBLE_ITEMS) + (itemsOnCurrentPage - 1);
                    int firstItemOnPage = currentPage * MAX_VISIBLE_ITEMS;

                    // Calculate new position considering rotation
                    int newPosition = settingsSelectedOption - event.totalSteps;

                    // Handle page transitions
                    if (event.totalSteps > 0) { // Scrolling up (towards higher indices)
                        if (settingsSelectedOption == lastItemOnPage && newPosition > lastItemOnPage) {
                            // At last item on page, scrolling up - go to next page
                            int nextPage = currentPage + 1;
                            if (nextPage * MAX_VISIBLE_ITEMS < SETTINGS_MENU_ITEM_COUNT) {
                                newPosition = nextPage * MAX_VISIBLE_ITEMS;
                            } else {
                                newPosition = lastItemOnPage; // Stay on current page
                            }
                        }
                    } else if (event.totalSteps < 0) { // Scrolling down (towards lower indices)
                        if (settingsSelectedOption == firstItemOnPage && newPosition < firstItemOnPage) {
                            // At first item on page, scrolling down - go to previous page
                            int prevPage = currentPage - 1;
                            if (prevPage >= 0) {
                                int prevPageLastItem = min((prevPage + 1) * MAX_VISIBLE_ITEMS - 1, SETTINGS_MENU_ITEM_COUNT - 1);
                                newPosition = prevPageLastItem;
                            } else {
                                newPosition = firstItemOnPage; // Stay on current page
                            }
                        }
                    }

                    // Clamp the position between 0 and SETTINGS_MENU_ITEM_COUNT - 1
                    if (newPosition < 0)
                        newPosition = 0;
                    if (newPosition >= SETTINGS_MENU_ITEM_COUNT)
                        newPosition = SETTINGS_MENU_ITEM_COUNT - 1;

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
                        hours = (hours - event.totalSteps + 24) % 24;
                        break;
                    case 1: // Minutes
                        minutes = (minutes - event.totalSteps + 60) % 60;
                        break;
                    case 2: // Seconds
                        seconds = (seconds - event.totalSteps + 60) % 60;
                        break;
                    }
                    displayClockSubmenu(nullptr);
                }
            }
        }

        switch (currentScreen)
        {
        case MainScreen:
            if (updateMainScreen)
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
                if (needsFullRedraw)
                {
                    displayDemo(parameters);
                    needsFullRedraw = false;
                }
            }
            break;

        case SettingsScreen:
            break;

        case ModulesSubmenu:
            if (needsFullRedraw || moduleConnectionStatus)
            {
                displayModulesSubmenu(parameters);
            }
            break;

        case RGBLightingSubmenu:
            displayRGBSubmenu(parameters);
            break;

            // Add cases for submenus here

        default:
            break;
        }

        xSemaphoreGive(screenMutex); // Release the mutex after updating the screen
        vTaskDelay(50 / portTICK_PERIOD_MS);
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