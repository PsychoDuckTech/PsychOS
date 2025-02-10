#include <Arduino.h>
#include "knobHandler.h"
#include "drivers/rotaryEncoder/KY-040.h"
#include "hostCommunicationBridge.h"
#include "displayHandler.h"

#define CLK_PIN 2
#define DT_PIN 36
#define SW_PIN 0
#define POLLING_RATE_MS 1 // 1 = 1000Hz, 2 = 500Hz

extern int moduleCount; // Add this line at the top

KY040 knob(CLK_PIN, DT_PIN, SW_PIN);

void displayRGBSubmenu(void *parameters);

void knobHandler(void *parameters)
{
    knob.begin();
    Serial.println("Knob Handler started");

    int lastRotation = 0;
    unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 50; // 50ms debounce delay

    for (;;)
    {
        int rotation = knob.readEncoder();
        bool longPress = knob.checkLongPress();
        bool shortPress = knob.checkButtonPress();
        bool doublePress = knob.checkButtonDoublePress();

        unsigned long currentTime = millis();

        if (rotation != lastRotation)
        {
            lastDebounceTime = currentTime;
            lastRotation = rotation;
        }

        if ((currentTime - lastDebounceTime) > debounceDelay)
        {
            if (rotation != 0)
            {
                handleRotation(rotation);
            }

            if (shortPress)
            {
                handleShortPress();
            }

            if (longPress)
            {
                handleLongPress();
            }

            if (doublePress)
            {
                handleDoublePress();
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
