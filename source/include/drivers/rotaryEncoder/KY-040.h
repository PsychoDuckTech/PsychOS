#ifndef KY_040_H
#define KY_040_H

#include <Arduino.h>

class KY040 {
private:
    const int _clkPin, _dtPin, _swPin;
    int _lastClk;
    int _lastDirection;
    unsigned long _lastDirectionTime;
    unsigned long _buttonPressStart;
    bool _buttonWasPressed;
    static const unsigned long DIRECTION_LOCK_TIME = 150; // ms
    static const unsigned long DEBOUNCE_TIME = 7;
    static const unsigned long BUTTON_DEBOUNCE = 50;

public:
    KY040(int clkPin, int dtPin, int swPin) 
        : _clkPin(clkPin), _dtPin(dtPin), _swPin(swPin),
          _lastClk(0), _lastDirection(0), _lastDirectionTime(0),
          _buttonPressStart(0), _buttonWasPressed(false) {}

    void begin() {
        pinMode(_clkPin, INPUT);
        pinMode(_dtPin, INPUT);
        pinMode(_swPin, INPUT_PULLUP);
        _lastClk = digitalRead(_clkPin);
    }

    int readEncoder() {
        int currentClk = digitalRead(_clkPin);
        
        if (currentClk != _lastClk) {
            int dtValue = digitalRead(_dtPin);
            int direction = (currentClk == dtValue) ? -1 : 1;
            unsigned long currentTime = millis();

            // If direction changed, check lock
            if (direction != _lastDirection) {
                // If previous direction was locked, block opposite
                if ((currentTime - _lastDirectionTime) < DIRECTION_LOCK_TIME) {
                    _lastClk = currentClk;
                    return 0;
                }
            }

            _lastClk = currentClk;
            _lastDirection = direction;
            _lastDirectionTime = currentTime;
            return direction;
        }
        return 0;
    }

    bool checkButtonPress() {
        bool currentState = digitalRead(_swPin) == LOW;
        unsigned long currentTime = millis();
        
        if (currentState && !_buttonWasPressed) {
            _buttonPressStart = currentTime;
            _buttonWasPressed = true;
            return false;
        }
        
        if (!currentState && _buttonWasPressed) {
            _buttonWasPressed = false;
            if ((currentTime - _buttonPressStart) > BUTTON_DEBOUNCE) {
                return true;
            }
        }
        
        return false;
    }

    bool isButtonPressed() {
        return digitalRead(_swPin) == LOW;
    }
};

#endif