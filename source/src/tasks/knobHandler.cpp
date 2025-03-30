#include <Arduino.h>
#include "tasks/knobHandler.h"
#include "drivers/rotaryEncoder/KY-040.h"
#include "tasks/hostCommunicationBridge.h"
#include "tasks/displayHandler.h"
#include "main.h"

#define POLLING_RATE_MS 1

KY040 knob(KNOB_CLK_PIN, KNOB_DT_PIN, KNOB_SW_PIN);

void displayRGBSubmenu(void *parameters);

void knobHandler(void *parameters)
{
    knob.begin();
    Serial.println("Knob Handler started");

    // Add variables for debouncing rapid rotations
    unsigned long lastRotationTime = 0;
    const unsigned long ROTATION_DEBOUNCE_MS = 50; // Minimum time between processing rotations

    for (;;)
    {
        int rotation = knob.readEncoder();
        bool longPress = knob.checkLongPress();
        bool shortPress = knob.checkButtonPress();
        bool doublePress = knob.checkButtonDoublePress();
        unsigned long currentTime = millis();

        if (currentScreen == MainScreen)
        {
            if (rotation != 0)
            {
                HostMessage msg;
                msg.type = VOLUME_CHANGE;
                msg.data = rotation;
                xQueueSend(hostMessageQueue, &msg, 0);
            }

            if (shortPress)
            {
                HostMessage msg;
                msg.type = VOLUME_MUTE;
                msg.data = 0;
                xQueueSend(hostMessageQueue, &msg, 0);
            }
        }

        if (currentScreen == SettingsScreen)
        {
            if (rotation != 0 && (currentTime - lastRotationTime) > ROTATION_DEBOUNCE_MS)
            {
                // Only process one step at a time with debouncing
                lastRotationTime = currentTime;

                // Get the direction of rotation (-1 for clockwise, 1 for counter-clockwise)
                // regardless of the magnitude
                int direction = rotation > 0 ? -1 : 1;

                // Move one step in the appropriate direction
                settingsSelectedOption = (settingsSelectedOption + direction) % 4;
                if (settingsSelectedOption < 0)
                    settingsSelectedOption = 3;
                displaySettingsScreen(nullptr);
            }

            if (shortPress)
            {
                switch (settingsSelectedOption)
                {
                case 0:
                    switchScreen(ModulesSubmenu);
                    break;
                case 1:
                    switchScreen(RGBLightingSubmenu);
                    break;
                case 2:
                    switchScreen(ClockSubmenu);
                    break;
                case 3:
                    switchScreen(IotSubmenu);
                    break;
                }
            }
        }

        if (currentScreen == RGBLightingSubmenu)
        {
            if (rotation != 0 && (currentTime - lastRotationTime) > ROTATION_DEBOUNCE_MS)
            {
                lastRotationTime = currentTime;
                int8_t delta = rotation > 0 ? 1 : -1;
                uint8_t *currentValue = &rgbState.values[rgbState.currentSelection];
                *currentValue = constrain(*currentValue + delta, 0, (rgbState.currentSelection == 3) ? 100 : 255);
                rgbState.needsRefresh = true;
            }

            if (shortPress)
            {
                rgbState.currentSelection = (rgbState.currentSelection + 1) % 4;
                rgbState.needsRefresh = true;
            }

            if (longPress)
            {
                firstDraw = true;
            }
        }

        if (currentScreen == ClockSubmenu)
        {
            if (rotation != 0 && (currentTime - lastRotationTime) > ROTATION_DEBOUNCE_MS)
            {
                lastRotationTime = currentTime;
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
                displayClockSubmenu(nullptr);
            }

            if (shortPress)
            {
                settingsSelectedOption = (settingsSelectedOption + 1) % 3;
                displayClockSubmenu(nullptr);
            }
        }

        if (longPress)
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

        if (doublePress)
        {
            if (currentScreen == MainScreen)
            {
                capsLockStatus = !capsLockStatus;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(POLLING_RATE_MS));
    }
}

void startKnobHandlerTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t knobTaskHandle;
    xTaskCreatePinnedToCore(
        knobHandler,
        "Knob Handler",
        stackDepth,
        NULL,
        priority,
        &knobTaskHandle,
        core);
}
