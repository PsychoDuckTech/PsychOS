#include <Arduino.h>
#include <ArduinoBLE.h>

void initializeBLE() {
    if (!BLE.begin()) {
        Serial.println("BLE initialization failed!");
        ESP.restart();
    }
    Serial.println("BLE initialized successfully");
}