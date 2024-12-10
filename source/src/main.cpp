#include <Arduino.h>
#include "boardConfiguration/prototype0.h" // The board specific configuration

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
    Serial.print("Started KeystrokeHandler Task\n");

    for (;;) {
        scanMatrix();
        vTaskDelay(1 / portTICK_PERIOD_MS); // 1ms delay corresponding to a polling rate of 1000Hz
    }
}

void scanMatrix() {
    for (int row; row < totalRows; row++) {
        digitalWrite(rowPins[row], HIGH);
        for (int col; col < totalCols; col++) {
            if (digitalRead(colPins[col]) == HIGH) {
                Serial.print("Key Pressed: " + String(keyMap[row][col]) + " Key name: " + keyName[row][col] + "\n");
                Serial.print("Row: " + String(row) + " Col: " + String(col) + "\n");
            }
        }
        digitalWrite(rowPins[row], LOW);
    }
}

void setupKeyboardMatrix(void *parameters) {
    for (int i = 0; i < totalRows; i++) {pinMode(rowPins[i], OUTPUT);} // Set all row GPIO as OUTPUT
    for (int i = 0; i < totalCols; i++) {pinMode(colPins[i], INPUT_PULLUP);} // Set all column GPIO as INPUT with PULLUP
}
// --------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    xTaskCreate(
        setupKeyboardMatrix,
        "Keyboard Matrix Setup",
        1000,
        NULL,
        7,
        NULL
    );
    xTaskCreate(
        KeystrokeHandler,
        "Keystroke Handler",
        1000,
        NULL,
        6,
        NULL
    );
}

void loop() {
    // nothing here
}