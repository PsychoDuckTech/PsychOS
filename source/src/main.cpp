#include <Arduino.h>

// Configurable Parameters -------------------------------------
#include "translations/enUS.h"
#include "boardConfig/prototype0.h" // The board specific configuration
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
    for (int i = 0; i < totalRows; i++) {
        pinMode(rowPins[i], OUTPUT);
    }
    for (int i = 0; i < totalCols; i++) {
        if (colPins[i] >= 34 && colPins[i] <= 39) {
            pinMode(colPins[i], INPUT); // No pull-up resistor
        } else {
            pinMode(colPins[i], INPUT_PULLUP);
        }
    }
    Serial.println(setupKeyboardMatrix_done);
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