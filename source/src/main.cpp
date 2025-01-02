#include <Arduino.h>
#include <ArduinoBLE.h>
#include "config.h"
#include "tasks/matrixScan.cpp"
#include "tasks/moduleConnectionHandler.cpp"
#include "tasks/knobHandler.cpp"
#include "utils/initializeMatrix.h"
#include "utils/initializeBLE.h"

void setup() {
    Serial.begin(115200);
    Serial.println(String(OS_version) + ", " + byCompany);
    Serial.println("95YCH0DUCK\n");
    
    // Create message queue first
    QueueHandle_t messageQueue = xQueueCreate(64, sizeof(HostMessage));
    if (!messageQueue) {
        Serial.println("Failed to create message queue!");
        return;
    }

    // Initialize watchdog
    esp_task_wdt_init(10, true);
    
    initializeBLE();
    initializeMatrix();
    
    TaskHandle_t keyTaskHandle;
    xTaskCreatePinnedToCore(
        matrixScan,           // Task function: The function that will execute as the task.
        "Keystroke Handler",  // Name of the task: A human-readable name for debugging.
        8192,                 // Stack size: Amount of stack memory allocated to the task (in bytes).
        NULL,        // Pass queue handle to task
        3,                    // Priority: The priority of the task (higher values = higher priority).
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
        1
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
        "Host Communication Bridge",
        4096,
        NULL,
        2,
        &hostCommHandle,
        0
    );
}

void loop() {
    // not required due to the use of FreeRTOS tasks
}