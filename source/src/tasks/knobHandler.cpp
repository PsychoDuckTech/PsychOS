#include <Arduino.h>
#include "tasks/knobHandler.h"
#include "drivers/rotaryEncoder/EncoderHandler.h"
#include "tasks/hostCommunicationBridge.h"
#include "tasks/displayHandler.h"
#include "main.h"

#define POLLING_RATE_MS 20

EncoderHandler knob(KNOB_CLK_PIN, KNOB_DT_PIN, KNOB_SW_PIN);

void knobHandler(void *parameters)
{
    knob.begin();
    Serial.println("Knob Handler started");

    unsigned long lastRotationTime = 0;
    const unsigned long ROTATION_DEBOUNCE_MS = 50;

    for (;;)
    {
        knob.update();
        int rotation = knob.getEncoderDelta();
        bool longPress = knob.checkLongPress();
        bool shortPress = knob.checkShortPress();
        bool doublePress = knob.checkDoublePress();
        unsigned long currentTime = millis();

        if (rotation != 0 && (currentTime - lastRotationTime) > ROTATION_DEBOUNCE_MS)
        {
            lastRotationTime = currentTime;

            // Handle rotation based on current screen
            switch (currentScreen)
            {
            case MainScreen:
                if (rotation != 0)
                {
                    HostMessage msg;
                    msg.type = VOLUME_CHANGE;
                    msg.data = rotation; // Natural direction for volume
                    xQueueSend(hostMessageQueue, &msg, 0);
                }
                break;

            case SettingsScreen:
            {
                SettingsRotationEvent event;
                event.totalSteps = rotation; // Natural direction
                xQueueSend(settingsRotationQueue, &event, 0);
            }
            break;

            case ClockSubmenu:
            {
                SettingsRotationEvent event;
                event.totalSteps = rotation; // Natural direction
                xQueueSend(settingsRotationQueue, &event, 0);
            }
            break;

            case RGBLightingSubmenu:
            {
                // Handle RGB menu navigation
                if (rgbState.currentSelection == 0) {
                    // Brightness adjustment (0-100 scale)
                    int oldBrightness = rgbState.brightness;
                    rgbState.brightness = constrain(rgbState.brightness + rotation, 0, 100);
                    
                    // Only update if value actually changed
                    if (oldBrightness != rgbState.brightness) {
                        // Send only the brightness command to avoid corrupting other settings
                        RGBCommand cmd;
                        cmd.type = RGB_CMD_SET_BRIGHTNESS;
                        cmd.data.brightness = rgbState.brightness;
                        
                        // Use a timeout instead of indefinite blocking
                        if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS) {
                            // If queue is full, log error but continue (don't block)
                            Serial.println("RGB queue full (brightness)");
                        }
                        
                        // Always refresh the display
                        rgbState.needsRefresh = true;
                    }
                } else {
                    // Speed adjustment (1-20 scale)
                    int oldSpeed = rgbState.speed;
                    rgbState.speed = constrain(rgbState.speed + rotation, 1, 20);
                    
                    // Only update if value actually changed
                    if (oldSpeed != rgbState.speed) {
                        // Send only the speed command
                        RGBCommand cmd;
                        cmd.type = RGB_CMD_SET_SPEED;
                        cmd.data.speed = rgbState.speed;
                        
                        // Use a timeout instead of indefinite blocking
                        if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS) {
                            // If queue is full, log error but continue (don't block)
                            Serial.println("RGB queue full (speed)");
                        }
                        
                        // Always refresh the display
                        rgbState.needsRefresh = true;
                    }
                }
            }
            break;
            }
        }

        // Handle button presses
        if (shortPress)
        {
            switch (currentScreen)
            {
            case MainScreen:
            {
                HostMessage msg;
                msg.type = VOLUME_MUTE;
                msg.data = 0;
                xQueueSend(hostMessageQueue, &msg, 0);
                break;
            }
            case SettingsScreen:
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
                break;
            case RGBLightingSubmenu:
                rgbState.currentSelection = (rgbState.currentSelection + 1) % 2; // Only 2 options now
                rgbState.needsRefresh = true;
                break;
            case ClockSubmenu:
                settingsSelectedOption = (settingsSelectedOption + 1) % 3;
                displayClockSubmenu(nullptr);
                break;
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

        if (doublePress && currentScreen == MainScreen)
        {
            capsLockStatus = !capsLockStatus;
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
