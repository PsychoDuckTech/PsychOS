#include <Arduino.h>
#include <ArduinoBLE.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "tasks/hostCommunicationBridge.cpp"
#include "tasks/keyScanning.cpp"
#include "tasks/moduleConnectionHandler.cpp"
#include "tasks/knobHandler.cpp"
#include "functions/initializeMatrix.h"
#include "functions/initializeBLE.h"

void setup() {
    Serial.begin(115200);
    Serial.println(String(OS_version) + ", " + byCompany + "\n");
    
    // Initialize watchdog
    esp_task_wdt_init(10, true);
    
    initializeBLE();
    initializeMatrix();
    
    USB.begin();
    
    TaskHandle_t keyTaskHandle;
    xTaskCreatePinnedToCore(
        keyScanning,          // Task function: The function that will execute as the task.
        "Keystroke Handler",  // Name of the task: A human-readable name for debugging.
        8192,                 // Stack size: Amount of stack memory allocated to the task (in bytes).
        NULL,                 // Parameter: A pointer passed as an argument to the task function.
        2,                    // Priority: The priority of the task (higher values = higher priority).
        &keyTaskHandle,       // Task handle: A pointer to store the task's handle (optional).
        0                     // Core ID: The CPU core (0 or 1) on which the task will run.
    );

    TaskHandle_t knobTaskHandle;
    xTaskCreatePinnedToCore(
        knobHandler,
        "Knob Handler",
        2048,
        NULL,
        1,
        &knobTaskHandle,
        0
    );

    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(
        moduleConnectionHandler,
        "BLE Handler",
        16384,
        NULL,
        1,
        &bleTaskHandle,
        1
    );

    TaskHandle_t hostCommHandle;
    xTaskCreatePinnedToCore(
        hostCommunicationBridge,
        "Host Communication",
        4096,
        NULL,
        1,
        &hostCommHandle,
        1
    );
}

void loop() {
    // not required due to the use of FreeRTOS tasks
}