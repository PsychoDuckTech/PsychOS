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
    static const unsigned long ENCODER_DEBOUNCE_TIME = 5; // 5ms debounce for encoder
    static const int ENCODER_THRESHOLD = 4;               // Number of steps before registering a change

    // Acceleration parameters
    static const unsigned long ACCEL_WINDOW_MS = 461; // Time window for acceleration (1 second)
    static const int ACCEL_THRESHOLD_STEPS = 3;       // Steps needed in window to trigger acceleration
    static const int MAX_ACCEL_MULTIPLIER = 3;        // Maximum acceleration multiplier

    // Track button states between updates
    bool lastButtonState = HIGH;
    bool currentButtonState = HIGH;
    unsigned long lastDebounceTime = 0;
    bool pendingShortPress = false;

    // Encoder state tracking
    int32_t lastStableCount = 0;
    int32_t accumulatedSteps = 0;
    unsigned long lastEncoderTime = 0;

    // Acceleration tracking
    unsigned long rotationTimes[10]; // Circular buffer to track last 10 rotation times
    int rotationTimesIndex = 0;      // Current index in circular buffer
    int rotationCount = 0;           // Count of rotations in the buffer

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
        lastEncoderTime = millis();
    }

    // Calculate acceleration multiplier based on rotation speed
    int calculateAcceleration()
    {
        unsigned long currentTime = millis();
        int stepsInWindow = 0;

        // Count rotations that happened within the acceleration window
        for (int i = 0; i < min(rotationCount, 10); i++)
        {
            if (currentTime - rotationTimes[i] < ACCEL_WINDOW_MS)
            {
                stepsInWindow++;
            }
        }

        // Apply acceleration if steps in window exceed threshold
        if (stepsInWindow >= ACCEL_THRESHOLD_STEPS)
        {
            // Scale the multiplier based on rotation speed
            // More steps in the window = higher multiplier (up to MAX_ACCEL_MULTIPLIER)
            int multiplier = 1 + min(stepsInWindow - ACCEL_THRESHOLD_STEPS + 1, MAX_ACCEL_MULTIPLIER - 1);
            return multiplier;
        }

        return 1; // No acceleration
    }

    int32_t getEncoderDelta()
    {
        unsigned long currentTime = millis();
        int32_t currentCount = encoder.getCount();

        // Only process changes after debounce time
        if (currentTime - lastEncoderTime >= ENCODER_DEBOUNCE_TIME)
        {
            int32_t delta = currentCount - lastStableCount;

            // Accumulate steps
            accumulatedSteps += delta;

            // If we've accumulated enough steps, return the movement
            if (abs(accumulatedSteps) >= ENCODER_THRESHOLD)
            {
                // Record this rotation time for acceleration tracking
                rotationTimes[rotationTimesIndex] = currentTime;
                rotationTimesIndex = (rotationTimesIndex + 1) % 10;
                rotationCount = min(rotationCount + 1, 10);

                // Calculate the base result
                int32_t baseResult = accumulatedSteps / ENCODER_THRESHOLD;
                accumulatedSteps = accumulatedSteps % ENCODER_THRESHOLD; // Keep remainder

                // Apply acceleration multiplier
                int multiplier = calculateAcceleration();
                int32_t result = baseResult * multiplier;

                lastStableCount = currentCount;
                lastEncoderTime = currentTime;

                // Debug output when acceleration is applied
                if (multiplier > 1)
                {
                    Serial.printf("Acceleration: %dx (steps in window: %d)\n",
                                  multiplier, min(rotationCount, 10));
                }

                return result;
            }

            lastStableCount = currentCount;
            lastEncoderTime = currentTime;
        }

        return 0;
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