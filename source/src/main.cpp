#include <Arduino.h>
#include "translations/ptPT.h"
#include "esp_task_wdt.h" // Include the watchdog timer header
#include "boardConfig/test2x2.h" // The board specific configuration

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
    Serial.printf("%d\n", task_keystrokeHandler_start);

    for (;;) {
        scanMatrix();
        esp_task_wdt_reset(); // Feed the watchdog timer
        vTaskDelay(100 / portTICK_PERIOD_MS); // 1ms delay corresponding to a polling rate of 1000Hz
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
    for (int i = 0; i < totalRows; i++) {pinMode(rowPins[i], OUTPUT);} // Set all row GPIO as OUTPUT
    for (int i = 0; i < totalCols; i++) {pinMode(colPins[i], INPUT_PULLUP);} // Set all column GPIO as INPUT with PULLUP
    Serial.println("Setting up Keyboard Matrix done.");
}

// void submitKey();
// --------------------------------------------------------------

void setup() {
    delay(500);
    Serial.begin(115200);
    setupKeyboardMatrix();

    // Initialize the task watchdog
    esp_task_wdt_init(10, true); // Timeout of 10 seconds

    // Add tasks to the watchdog
    esp_task_wdt_add(NULL);

    xTaskCreate(
        KeystrokeHandler,
        "Keystroke Handler",
        4096,
        NULL,
        1,
        NULL
    );
}

void loop() {
    // nothing here
}