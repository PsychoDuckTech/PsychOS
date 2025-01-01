#pragma once
#include <Arduino.h>

// Standard HID report structure
struct matrixReport {
    uint8_t modifiers;  // Ctrl, Shift, Alt, GUI keys
    uint8_t reserved;   // Reserved byte
    uint8_t keys[6];   // Regular keys (6KRO)
};

class customHID {
public:
    void begin(void);
    void sendReport(matrixReport* report);
    void releaseAll(void);
    
private:
    matrixReport _report = {0};
    static const uint8_t REPORT_ID = 1;
};

extern customHID Keyboard;