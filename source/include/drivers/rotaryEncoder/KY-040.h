#ifndef KY_040_H
#define KY_040_H

#include <Arduino.h>

class KY040
{
private:
    const int _clkPin, _dtPin, _swPin;
    int _lastClk;
    int _lastDirection;
    unsigned long _lastDirectionTime;
    unsigned long _buttonPressStart;
    bool _buttonWasPressed;
    bool _longPressEventDetected;
    static const unsigned long DIRECTION_LOCK_TIME = 200; // ms
    static const unsigned long BUTTON_DEBOUNCE = 50;
    static const unsigned long LONG_PRESS_TIME = 225; // ms

    // Double-tap detection variables
    static const unsigned long DOUBLE_PRESS_TIMEOUT = 250; // ms
    bool _pendingSinglePress = false;
    unsigned long _pendingSinglePressTime = 0;
    bool _doublePressDetected = false;

public:
    KY040(int clkPin, int dtPin, int swPin)
        : _clkPin(clkPin), _dtPin(dtPin), _swPin(swPin),
          _lastClk(0), _lastDirection(0), _lastDirectionTime(0),
          _buttonPressStart(0), _buttonWasPressed(false),
          _longPressEventDetected(false) {}

    void begin()
    {
        pinMode(_clkPin, INPUT);
        pinMode(_dtPin, INPUT);
        pinMode(_swPin, INPUT_PULLUP);
        _lastClk = digitalRead(_clkPin);
    }

    int readEncoder()
    {
        int currentClk = digitalRead(_clkPin);

        if (currentClk != _lastClk)
        {
            int dtValue = digitalRead(_dtPin);
            int direction = (currentClk == dtValue) ? -1 : 1;
            unsigned long currentTime = millis();

            // If direction changed, check lock
            if (direction != _lastDirection)
            {
                // If previous direction was locked, return previous direction
                if ((currentTime - _lastDirectionTime) < DIRECTION_LOCK_TIME)
                {
                    _lastClk = currentClk;
                    return _lastDirection;
                }
            }

            // Update the last state and direction
            _lastClk = currentClk;
            _lastDirection = direction;
            _lastDirectionTime = currentTime;

            // Check for rapid direction changes
            if ((currentTime - _lastDirectionTime) < DIRECTION_LOCK_TIME)
            {
                // If a rapid change is detected, return the previous direction
                return _lastDirection;
            }

            return direction;
        }
        return 0;
    }

    bool checkButtonPress()
    {
        bool currentState = digitalRead(_swPin) == LOW;
        unsigned long currentTime = millis();

        if (currentState && !_buttonWasPressed)
        {
            _buttonPressStart = currentTime;
            _buttonWasPressed = true;
            _longPressEventDetected = false; // Reset long press flag on new press
            return false;
        }

        if (!currentState && _buttonWasPressed)
        {
            unsigned long pressDuration = currentTime - _buttonPressStart;
            _buttonWasPressed = false;

            if (pressDuration >= BUTTON_DEBOUNCE)
            {
                if (pressDuration < LONG_PRESS_TIME)
                {
                    // Short press detected
                    if (!_pendingSinglePress)
                    {
                        // First press in potential double-tap
                        _pendingSinglePress = true;
                        _pendingSinglePressTime = currentTime;
                    }
                    else
                    {
                        // Second press within the timeout
                        if (currentTime - _pendingSinglePressTime <= DOUBLE_PRESS_TIMEOUT)
                        {
                            _doublePressDetected = true;
                            _pendingSinglePress = false;
                        }
                        else
                        {
                            // Second press after timeout, treat as new single press
                            _pendingSinglePress = true;
                            _pendingSinglePressTime = currentTime;
                        }
                    }
                }
                else
                {
                    // Long press detected
                    _longPressEventDetected = true;
                }
            }
            return false;
        }

        // Check if pending single press has timed out
        if (_pendingSinglePress && (currentTime - _pendingSinglePressTime) > DOUBLE_PRESS_TIMEOUT)
        {
            _pendingSinglePress = false;
            return true;
        }

        return false;
    }

    bool checkButtonDoublePress()
    {
        if (_doublePressDetected)
        {
            _doublePressDetected = false;
            return true;
        }
        return false;
    }

    bool checkLongPress()
    {
        if (_longPressEventDetected)
        {
            _longPressEventDetected = false;
            return true;
        }
        return false;
    }

    bool isButtonPressed()
    {
        return digitalRead(_swPin) == LOW;
    }
};

#endif