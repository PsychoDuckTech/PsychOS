#include <Arduino.h>
#include "config.h"
#include "drivers/rotaryEncoder/KY-040.h"
#include "hostCommunicationBridge.h"
#include "displayHandler.h"

#define CLK_PIN 2
#define DT_PIN 36
#define SW_PIN 0
#define POLLING_RATE_MS 1 // 1 = 1000Hz, 2 = 500Hz

KY040 knob(CLK_PIN, DT_PIN, SW_PIN);

void knobHandler(void *parameters)
{
    knob.begin();
    Serial.println("Knob Handler started");

    for (;;)
    {
        int rotation = knob.readEncoder();
        bool longPress = knob.checkLongPress();
        bool shortPress = knob.checkButtonPress();

        if (currentScreen == SettingsScreen)
        {
            // Handle settings navigation
            if (rotation != 0)
            {
                settingsSelectedOption = (settingsSelectedOption + (rotation > 0 ? 1 : -1)) % 4;
                if (settingsSelectedOption < 0)
                    settingsSelectedOption = 3;
                displaySettingsScreen(nullptr); // Refresh display
            }

            if (shortPress)
            {
                // Handle submenu entry
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
                    switchScreen(RGBSubmenu);
                    break;
                }
            }
        }
        else if (currentScreen == MainScreen)
        {
            // Original volume handling
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

        // Long press handling
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

        vTaskDelay(pdMS_TO_TICKS(POLLING_RATE_MS));
    }
}
