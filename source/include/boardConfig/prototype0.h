/**
 * Prototype 0 Keyboard Matrix Configuration
 */

#include <Arduino.h>
#include "usb_hid_keys.h" // by MightyPork on GitHub, customized by GabiBrawl
#include "multiplexer.h"

// Matrix configuration
int const totalRows = 6;
int const totalCols = 16;
int const usesMultiplexer = true;

// GPIO assignment
int const rowPins[totalRows] = {40, 13, 12, 11, 10, 9};
// int const colPins[totalCols] = {22, 23, 24, 25, 27, 28, 44, 43, 31, 32, 33, 34, 35, 36, 37}; // DONT USE 14, 16, 15, 46, 1, 17, 30

// 16bit multiplexer pins and instance
#define MULTIPLEXER_S0 5
#define MULTIPLEXER_S1 6
#define MULTIPLEXER_S2 7
#define MULTIPLEXER_S3 15
#define MULTIPLEXER_SIG 4
Multiplexer colPinsMultiplexer(MULTIPLEXER_S0, MULTIPLEXER_S1, MULTIPLEXER_S2, MULTIPLEXER_S3, MULTIPLEXER_SIG);

// Matrix Key Map
const uint8_t keyMap[totalRows][totalCols] = {
    {KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_SYSRQ, KEY_INSERT, KEY_DELETE}, // done
    {KEY_BACKSLASH, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_APOSTROPHE, 0, KEY_BACKSPACE, 0, KEY_HOME}, // missing one key
    {KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_PLUS_ASTERISK, KEY_ACUTE_GRAVE, 0, 0, KEY_END}, // done, one empty key
    {KEY_CAPSLOCK, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_C_CEDILLA, KEY_ORDINAL_MASCULINE, KEY_CIRCUMFLEX_TILDE, KEY_ENTER, 0, KEY_PAGEUP}, // done
    {KEY_LEFTSHIFT, KEY_LESS_GREATER, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_DOT, KEY_MINUS_UNDERSCORE, KEY_RIGHT_SHIFT, 0, KEY_UP, KEY_PAGEDOWN}, // done
    {KEY_LEFTCTRL, KEY_LEFTMETA, 0, KEY_LEFTALT, 0, 0, KEY_SPACE, 0, 0, 0, KEY_RIGHTALT, 0, KEY_RIGHTCTRL, KEY_LEFT, KEY_DOWN, KEY_RIGHT} // done
};

// Matrix Key Name
const char* keyName[totalRows][totalCols] = {
    {"ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "SYSRQ", "INSERT", "DELETE"},
    {"BACKSLASH", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "APOSTROPHE", "", "BACKSPACE", "", "HOME"},
    {"TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "EQUAL", "GRAVE", "", "", "END"},
    {"CAPSLOCK", "A", "S", "D", "F", "G", "H", "J", "K", "L", "C_CEDILLA", "ORDINAL_MASCULINE", "CIRCUMFLEX_TILDE", "ENTER", "", "PAGEUP"},
    {"LEFTSHIFT", "LESS_GREATER", "Z", "X", "C", "V", "B", "N", "M", "COMMA", "DOT", "MINUS", "RIGHT_SHIFT", "", "UP", "PAGEDOWN"},
    {"LEFTCTRL", "LEFTMETA", "", "LEFTALT", "", "", "SPACE", "", "", "", "RIGHTALT", "", "RIGHTCTRL", "LEFT", "DOWN", "RIGHT"}
};

// Board Configuration Specific Tasks ----------------------------------------
void scanMatrixLogic() {
    for (int row = 0; row < totalRows; row++) {
        digitalWrite(rowPins[row], HIGH);
        
        // Scan columns through the multiplexer
        for (int col = 0; col < totalCols; col++) {
            colPinsMultiplexer.selectChannel(col);
            delayMicroseconds(50);
            
            if (!colPinsMultiplexer.readChannel()) {  // Activate on low
                Serial.printf("Key Pressed: %s\n", keyName[row][col]);
                Serial.printf("Row: %d Col: %d\n", row, col);
            }
        }
        digitalWrite(rowPins[row], LOW);
    }
}

void setupColPins() {
    Serial.println("No column pins to setup here");
}