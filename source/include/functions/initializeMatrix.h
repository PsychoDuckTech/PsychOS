#include <Arduino.h>
#include "config.h"

void setupRowPins();
void setupColPins();

void initializeMatrix() {
    setupRowPins();
    setupColPins(); // Present in the board specific configuration
    Serial.println(matrix_initialized_successfully);
}

void setupRowPins() {
    for (int i = 0; i < totalRows; i++) {
        // Serial.printf("Starting row pin %d (GPIO%d)... ", i, rowPins[i]);
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], LOW);
        // Serial.printf("done\n");
    }
}

void setupColPins() {
    switch (usesMultiplexer) {
        case true:
            // Serial.println("No column pins to setup here");
            break;
        case false:
            for (int i = 0; i < totalCols; i++) {
                // Serial.printf("\nStarting column pin %d (GPIO%d) setup\n", i, colPins[i]);
                pinMode(colPins[i], INPUT);
                // Serial.printf("Completed column pin %d setup\n", i);
            }
            break;
    }
}