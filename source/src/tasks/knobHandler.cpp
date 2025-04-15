#include <Arduino.h>
#include "tasks/knobHandler.h"
#include "drivers/rotaryEncoder/EncoderHandler.h"
#include "tasks/hostCommunicationBridge.h"
#include "tasks/displayHandler.h"
#include "main.h"
#include "display/screens.h"
extern void displayPixelFlushScreen();
extern bool needsFullRedraw;

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

            case PixelFlushScreen:
                // Adjust volume on rotation
                if (rotation != 0)
                {
                    HostMessage msg;
                    msg.type = VOLUME_CHANGE;
                    msg.data = rotation; // Natural direction for volume
                    xQueueSend(hostMessageQueue, &msg, 0);
                }

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
                if (rgbState.currentSelection == 0)
                {
                    // Effect selection (cycle through effect types)
                    int oldEffect = rgbState.effect;
                    // Calculate new effect index, wrapping around at boundaries
                    // We have 5 effects (0-4)
                    int newEffect = rgbState.effect + rotation;
                    if (newEffect < 0)
                    {
                        newEffect = 4; // Wrap to last effect
                    }
                    else if (newEffect > 4)
                    {
                        newEffect = 0; // Wrap to first effect
                    }
                    rgbState.effect = newEffect;

                    // Only update if value actually changed
                    if (oldEffect != rgbState.effect)
                    {
                        // Send effect change command
                        RGBCommand cmd;
                        cmd.type = RGB_CMD_SET_EFFECT;

                        // Create different effect configurations based on selected effect
                        switch (rgbState.effect)
                        {
                        case RGB_EFFECT_STATIC:
                            cmd.data.effect.config = {RGB_EFFECT_STATIC,
                                                      static_cast<uint8_t>(map(rgbState.speed, 1, 20, 1, 255)), 255};
                            strncpy(cmd.data.effect.colors[0], "#FFA500", HEX_COLOR_LENGTH); // Orange
                            cmd.data.effect.num_colors = 1;
                            break;

                        case RGB_EFFECT_RUNNER:
                            cmd.data.effect.config = {RGB_EFFECT_RUNNER,
                                                      static_cast<uint8_t>(map(rgbState.speed, 1, 20, 1, 255)), 255};
                            strncpy(cmd.data.effect.colors[0], "#FFA500", HEX_COLOR_LENGTH); // Orange
                            strncpy(cmd.data.effect.colors[1], "#FF69B4", HEX_COLOR_LENGTH); // Hot Pink
                            strncpy(cmd.data.effect.colors[2], "#800080", HEX_COLOR_LENGTH); // Purple
                            cmd.data.effect.num_colors = 3;
                            break;

                        case RGB_EFFECT_SCROLL:
                            cmd.data.effect.config = {RGB_EFFECT_SCROLL,
                                                      static_cast<uint8_t>(map(rgbState.speed, 1, 20, 1, 255)), 255};
                            strncpy(cmd.data.effect.colors[0], "#1e130c", HEX_COLOR_LENGTH); // Dark Brown
                            strncpy(cmd.data.effect.colors[1], "#9a8478", HEX_COLOR_LENGTH); // Light Brown
                            cmd.data.effect.num_colors = 2;
                            break;

                        case RGB_EFFECT_BREATHE:
                            cmd.data.effect.config = {RGB_EFFECT_BREATHE,
                                                      static_cast<uint8_t>(map(rgbState.speed, 1, 20, 1, 255)), 255};
                            strncpy(cmd.data.effect.colors[0], "#00BFFF", HEX_COLOR_LENGTH); // Deep Sky Blue
                            strncpy(cmd.data.effect.colors[1], "#4B0082", HEX_COLOR_LENGTH); // Indigo
                            cmd.data.effect.num_colors = 2;
                            break;

                        default:                                          // RGB_EFFECT_WAVE (custom effect for demo)
                            cmd.data.effect.config = {RGB_EFFECT_BREATHE, // Using breathe as base
                                                      static_cast<uint8_t>(map(rgbState.speed, 1, 20, 1, 255)), 255};
                            strncpy(cmd.data.effect.colors[0], "#00FF00", HEX_COLOR_LENGTH); // Green
                            strncpy(cmd.data.effect.colors[1], "#0000FF", HEX_COLOR_LENGTH); // Blue
                            strncpy(cmd.data.effect.colors[2], "#FF0000", HEX_COLOR_LENGTH); // Red
                            cmd.data.effect.num_colors = 3;
                            break;
                        }

                        cmd.data.effect.set_colors = true;
                        cmd.data.effect.temporary = false;
                        cmd.data.effect.duration_ms = 0;

                        // Use a timeout instead of indefinite blocking
                        if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
                        {
                            // If queue is full, log error but continue (don't block)
                            Serial.println("RGB queue full (effect)");
                        }

                        // Always refresh the display
                        rgbState.needsRefresh = true;
                    }
                }
                else if (rgbState.currentSelection == 1)
                {
                    // Brightness adjustment (0-100 scale)
                    int oldBrightness = rgbState.brightness;
                    rgbState.brightness = constrain(rgbState.brightness + rotation, 0, 100);

                    // Only update if value actually changed
                    if (oldBrightness != rgbState.brightness)
                    {
                        // Send only the brightness command
                        RGBCommand cmd;
                        cmd.type = RGB_CMD_SET_BRIGHTNESS;
                        cmd.data.brightness = rgbState.brightness;

                        // Use a timeout instead of indefinite blocking
                        if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
                        {
                            // If queue is full, log error but continue (don't block)
                            Serial.println("RGB queue full (brightness)");
                        }

                        // Always refresh the display
                        rgbState.needsRefresh = true;
                    }
                }
                else
                {
                    // Speed adjustment (1-20 scale)
                    int oldSpeed = rgbState.speed;
                    rgbState.speed = constrain(rgbState.speed + rotation, 1, 20);

                    // Only update if value actually changed
                    if (oldSpeed != rgbState.speed)
                    {
                        // Send only the speed command
                        RGBCommand cmd;
                        cmd.type = RGB_CMD_SET_SPEED;
                        cmd.data.speed = rgbState.speed;

                        // Use a timeout instead of indefinite blocking
                        if (xQueueSend(rgbCommandQueue, &cmd, pdMS_TO_TICKS(50)) != pdPASS)
                        {
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
                    switchScreen(PixelFlushScreen);
                    break;
                }
                break;
            case RGBLightingSubmenu:
                rgbState.currentSelection = (rgbState.currentSelection + 1) % 3; // Now 3 options (Effect, Brightness, Speed)
                rgbState.needsRefresh = true;
                break;
            case ClockSubmenu:
                settingsSelectedOption = (settingsSelectedOption + 1) % 3;
                displayClockSubmenu(nullptr);
                break;
            case PixelFlushScreen:
                switchScreen(MainScreen);
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
