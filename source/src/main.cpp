#include <Arduino.h>

// Configurable Parameters -------------------------------------
#include "translations/enUS.h"
#include "boardConfig/2x2.h" // The board specific configuration
// -------------------------------------------------------------

typedef struct {
    uint8_t modifier;  // First byte for modifier keys
    uint8_t reserved;  // Second byte (always 0)
    uint8_t keycode[6];  // Up to 6 simultaneous key slots
} USB_HID_Keyboard_Report_t;

// Function Declarations ----------------------------------------
void scanMatrix();
void setupKeyboardMatrix(void *parameters);


//TASKS ---------------------------------------------------------
void KeystrokeHandler(void *parameters) {
    USB_HID_Keyboard_Report_t hid_report;
    Serial.println(task_keystrokeHandler_started);

    for (;;) {
        scanMatrix();
        vTaskDelay(1 / portTICK_PERIOD_MS); // 1ms delay corresponding to a polling rate of 1000Hz
    }
}

void scanMatrix() {
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

void setupKeyboardMatrix() {
    Serial.println("Setting up Keyboard Matrix");
    
    // Debug row pins with increased delay and verification
    Serial.println("Setting up row pins...");
    for (int i = 0; i < totalRows; i++) {
        Serial.printf("Starting row pin %d (GPIO%d) setup\n", i, rowPins[i]);
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], LOW);
        Serial.printf("Completed row pin %d setup\n", i);
    }
    Serial.println("All row pins setup complete");
    
    // Debug column pins in chunks
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
        
        // Add extra delay every 4 pins
        if (i % 4 == 3) {
            Serial.println("Taking a break...");
        }
    }
    
    Serial.println("\nAll pins setup complete");
    Serial.println(setupKeyboardMatrix_done);
}

void scanMatrix() {
    for (int row = 0; row < totalRows; row++) {
        digitalWrite(rowPins[row], HIGH);
        
        // Scan all columns through multiplexer
        for (int col = 0; col < totalCols; col++) {
            colMux.selectChannel(col);
            delayMicroseconds(5);  // Small delay for multiplexer switching
            
            if (!colMux.readChannel()) {  // Active low
                Serial.printf("Key Pressed: %s\n", keyName[row][col]);
                Serial.printf("Row: %d Col: %d\n", row, col);
            }
        }
        digitalWrite(rowPins[row], LOW);
    }
}

// void submitKey();
// --------------------------------------------------------------

void setup() {
    delay(2000);
    Serial.begin(115200);
    Serial.println("Setup started");
    setupKeyboardMatrix();


    xTaskCreate(
        KeystrokeHandler,
        "Keystroke Handler",
        8192,
        NULL,
        1,
        NULL
    );
}

void loop() {
    // nothing here
}