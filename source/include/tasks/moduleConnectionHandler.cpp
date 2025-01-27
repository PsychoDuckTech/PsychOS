#include <Arduino.h>
#include <ArduinoBLE.h>
#include "config.h"
#include "globals.h"
#include "utils/initializeBLE.h"

BLEService psychoService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEStringCharacteristic psychoCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 512);

void moduleConnectionHandler(void *parameter)
{
    initializeBLE();

    bool wasConnected = false;

    // Setup BLE service and characteristic
    BLE.setLocalName("Prototype0");
    BLE.setAdvertisedService(psychoService);
    psychoService.addCharacteristic(psychoCharacteristic);
    BLE.addService(psychoService);
    BLE.advertise();

    Serial.println(String(task_moduleConnectionHandler_started) + waitingForConnection);

    for (;;)
    {
        BLE.poll();
        bool isConnected = BLE.connected();

        if (isConnected && !wasConnected)
        {
            Serial.println(moduleConnected);
            moduleConnectionStatus = 1;
            vTaskDelay(2000 / portTICK_PERIOD_MS); // Wait for the client to be ready. DONT REMOVE
        }
        else if (!isConnected && wasConnected)
        {
            Serial.println(moduleDisconnected);
            moduleConnectionStatus = 0;
        }

        if (isConnected)
        {
            if (psychoCharacteristic.written())
            {
                String message = psychoCharacteristic.value();
                Serial.println(received + message);
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        else
        {
            vTaskDelay(1500 / portTICK_PERIOD_MS);
        }

        wasConnected = isConnected;
    }
}

void startBleTask(UBaseType_t core = 1, uint32_t stackDepth = 16384, UBaseType_t priority = 1)
{
    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(
        moduleConnectionHandler, // Function to be called
        "BLE Handler",           // Name of the task
        stackDepth,              // Stack size in words
        NULL,                    // Task input parameter
        priority,                // Priority of the task
        &bleTaskHandle,          // Task handle
        core                     // Core where the task should run
    );
}