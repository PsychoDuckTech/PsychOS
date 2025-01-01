#include <Arduino.h>
#include <ArduinoBLE.h>

void initializeBLE() {
    if (!BLE.begin()) {
        Serial.println(BLE_initialized_successfully); 
        ESP.restart();
    }
    Serial.println(BLE.begin());
    Serial.println(BLE_initialization_failed);
}