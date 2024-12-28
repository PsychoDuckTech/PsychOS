#ifndef KY_040_H
#define KY_040_H

#include <Arduino.h>

class KY040 {
public:
    KY040(int clkPin, int dtPin, int swPin);
    void begin();
    int readEncoder();
    bool isButtonPressed();

private:
    int _clkPin;
    int _dtPin;
    int _swPin;
    int _lastState;
    int _currentState;
};

KY040::KY040(int clkPin, int dtPin, int swPin)
    : _clkPin(clkPin), _dtPin(dtPin), _swPin(swPin), _lastState(LOW), _currentState(LOW) {}

void KY040::begin() {
    pinMode(_clkPin, INPUT);
    pinMode(_dtPin, INPUT);
    pinMode(_swPin, INPUT_PULLUP);
    _lastState = digitalRead(_clkPin);
}

int KY040::readEncoder() {
    _currentState = digitalRead(_clkPin);
    if (_currentState != _lastState) {
        if (digitalRead(_dtPin) != _currentState) {
            _lastState = _currentState;
            return 1; // Clockwise
        } else {
            _lastState = _currentState;
            return -1; // Counter-clockwise
        }
    }
    return 0; // No movement
}

bool KY040::isButtonPressed() {
    return digitalRead(_swPin) == LOW;
}

#endif // KY_040_H