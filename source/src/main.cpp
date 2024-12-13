#include <Arduino.h>
// Configurable Libraries --------------------------------------
#include "translations/enUS.h"
#include "boardConfig/prototype0.h" // The board specific configuration

typedef struct {
    uint8_t modifier;  // First byte for modifier keys
    uint8_t reserved;  // Second byte (always 0)
    uint8_t keycode[6];  // Up to 6 simultaneous key slots
} USB_HID_Keyboard_Report_t;

// Function Declarations ----------------------------------------
void setupKeyboardMatrix(void *parameters);
void scanMatrixLogic();
void setupRowPins();


//TASKS ---------------------------------------------------------
void KeystrokeHandler(void *parameters) {
    USB_HID_Keyboard_Report_t hid_report;
    Serial.println(task_keystrokeHandler_started);

    for (;;) {
        scanMatrixLogic(); // Present in the board specific configuration
        vTaskDelay(500 / portTICK_PERIOD_MS); // 1ms delay corresponding to a polling rate of 1000Hz
    }
}

void setupKeyboardMatrix() {
    Serial.println("Starting up Keyboard Matrix...\n");
    setupRowPins();
    setupColPins(); // Present in the board specific configuration
    Serial.println(setupKeyboardMatrix_done);
}

void setupRowPins() {
    for (int i = 0; i < totalRows; i++) {
        Serial.printf("Starting row pin %d (GPIO%d)... ", i, rowPins[i]);
        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], LOW);
        Serial.printf("done\n");
    }
}

// void submitKey();
// --------------------------------------------------------------

void setup() {
    delay(1500);
    Serial.begin(115200);
    Serial.println(String(OS_version) + ", " + byCompany + "\n");
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