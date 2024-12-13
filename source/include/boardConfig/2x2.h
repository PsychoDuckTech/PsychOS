/**
 * Key Array Map for the 2x2 Matrix
 */

#include <Arduino.h>
#include "usb_hid_keys.h" // by MightyPork on GitHub

// Matrix configuration
int const totalRows = 2;
int const totalCols = 2;
int const usesMultiplexer = false;

// Matrix Key Map
const uint8_t keyMap[totalRows][totalCols] = {
    {KEY_ESC, KEY_F1},
    {KEY_1, KEY_2}
};

// Matrix Key Name
const char* keyName[totalRows][totalCols] = {
    {"ESC", "F1"},
    {"1", "2"}
};

// GPIO assignment
int const rowPins[totalRows] = {4, 5};
int const colPins[totalCols] = {9, 6};

// Board Configuration Specific Tasks ----------------------------------------
void setupColPins() {
    Serial.println("Setting up column pins...");
    for (int i = 0; i < totalCols; i++) {
        Serial.printf("\nStarting column pin %d (GPIO%d) setup\n", i, colPins[i]);
        
        if (colPins[i] >= 34 && colPins[i] <= 39) {
            Serial.printf("Setting pin %d as input-only\n", colPins[i]);
            pinMode(colPins[i], INPUT);
        } else {
            Serial.printf("Setting pin %d with pullup\n", colPins[i]);
            pinMode(colPins[i], INPUT_PULLUP);
        }
        
        Serial.printf("Completed column pin %d setup\n", i);
    }
    Serial.println("All column pins setup complete");
}

void scanMatrixLogic() {
    for (int row = 0; row < totalRows; row++) {
        digitalWrite(rowPins[row], HIGH);
        for (int col = 0; col < totalCols; col++) {
            if (digitalRead(colPins[col]) == LOW) {
                Serial.printf("Key Pressed: %s\n", keyName[row][col]);
                Serial.printf("Row: %d Col: %d\n", row, col);
            }
        }
        digitalWrite(rowPins[row], LOW);
    }
}