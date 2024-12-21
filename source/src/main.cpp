#include <Arduino.h>
#include <ArduinoBLE.h>
#include <esp_task_wdt.h>
#include "tasks/keyScanning.cpp"
#include "tasks/moduleConnectionHandler.cpp"
#include "config.h"
#include "functions/initializeMatrix.h"
#include "functions/initializeBLE.h"

void setup() {
    delay(1500);
    Serial.begin(115200);
    Serial.println(String(OS_version) + ", " + byCompany + "\n");
    
    // Initialize watchdog
    esp_task_wdt_init(10, true);
    
    initializeBLE();
    initializeMatrix();


    // Create tasks with increased stack
    TaskHandle_t keyTaskHandle;
    TaskHandle_t bleTaskHandle;

    xTaskCreatePinnedToCore(
        keyScanning,
        "Keystroke Handler",
        8192,
        NULL,
        2,
        &keyTaskHandle,
        0
    );

    xTaskCreatePinnedToCore(
        moduleConnectionHandler,
        "BLE Handler",
        16384,
        NULL,
        1,
        &bleTaskHandle,
        1
    );
}

void loop() {
}