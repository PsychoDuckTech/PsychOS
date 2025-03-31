#pragma once

#include <Arduino.h>
#include <ESP32Encoder.h>

class EncoderHandler
{
private:
    ESP32Encoder encoder;
    const int buttonPin;
    volatile bool buttonPressed = false;
    volatile bool buttonReleased = false;
    volatile unsigned long pressStartTime = 0;
    volatile bool longPressDetected = false;
    volatile bool doublePressDetected = false;
    volatile unsigned long lastPressTime = 0;

    static const unsigned long DEBOUNCE_TIME = 50;
    static const unsigned long LONG_PRESS_TIME = 225;
    static const unsigned long DOUBLE_PRESS_TIMEOUT = 250;

    // Track button states between updates
    bool lastButtonState = HIGH;
    bool currentButtonState = HIGH;
    unsigned long lastDebounceTime = 0;
    bool pendingShortPress = false;

public:
    EncoderHandler(int clkPin, int dtPin, int swPin) : buttonPin(swPin)
    {
        encoder.attachFullQuad(clkPin, dtPin);
        encoder.setCount(0);
        pinMode(buttonPin, INPUT_PULLUP);
    }

    void begin()
    {
        ESP32Encoder::useInternalWeakPullResistors = UP;
        encoder.clearCount();
        encoder.setFilter(1023);
    }

    int32_t getEncoderDelta()
    {
        static int32_t lastCount = 0;
        int32_t currentCount = encoder.getCount();
        int32_t delta = currentCount - lastCount;
        lastCount = currentCount;
        return delta;
    }

    void update()
    {
        bool reading = digitalRead(buttonPin);
        unsigned long currentTime = millis();

        // Reset states
        buttonPressed = false;
        buttonReleased = false;

        // Debounce button
        if (reading != lastButtonState)
        {
            lastDebounceTime = currentTime;
        }

        if ((currentTime - lastDebounceTime) > DEBOUNCE_TIME)
        {
            if (reading != currentButtonState)
            {
                currentButtonState = reading;

                if (currentButtonState == LOW)
                { // Button pressed
                    buttonPressed = true;
                    pressStartTime = currentTime;

                    // Check for double press
                    if ((currentTime - lastPressTime) < DOUBLE_PRESS_TIMEOUT)
                    {
                        doublePressDetected = true;
                        pendingShortPress = false;
                    }
                    lastPressTime = currentTime;
                }
                else
                { // Button released
                    buttonReleased = true;
                    if ((currentTime - pressStartTime) >= LONG_PRESS_TIME)
                    {
                        longPressDetected = true;
                    }
                    else if (!doublePressDetected)
                    {
                        pendingShortPress = true;
                    }
                }
            }
        }

        lastButtonState = reading;

        // Clear pending short press if double press timeout reached
        if (pendingShortPress && (currentTime - lastPressTime) >= DOUBLE_PRESS_TIMEOUT)
        {
            buttonReleased = true;
            pendingShortPress = false;
        }
    }

    bool checkShortPress()
    {
        if (buttonReleased && pendingShortPress && !longPressDetected && !doublePressDetected)
        {
            pendingShortPress = false;
            buttonReleased = false;
            return true;
        }
        return false;
    }

    bool checkLongPress()
    {
        if (longPressDetected)
        {
            longPressDetected = false;
            return true;
        }
        return false;
    }

    bool checkDoublePress()
    {
        if (doublePressDetected && buttonReleased)
        {
            doublePressDetected = false;
            return true;
        }
        return false;
    }
};