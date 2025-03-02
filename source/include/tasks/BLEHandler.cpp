#include <Arduino.h>
#include <ArduinoBLE.h>
#include "BLEHandler.h"
#include "globals.h"
#include "utils/initializeBLE.h"
#include "tasks/matrixScan.h"
#include "tasks/buzzer.h"

/*
Target data flow: Slave Matrix -> BLE -> Master BLE -> Host Bridge -> USB HID
*/

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-000000000001"

BLEService psychoService(SERVICE_UUID);
BLECharacteristic psychoCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 20);

void BLEHandler(void *parameter)
{
    initializeBLE();

    bool wasConnected = false;

    BLE.setLocalName("Kibodo one");
    BLE.setDeviceName("Kibodo one");
    psychoService.addCharacteristic(psychoCharacteristic);
    BLE.addService(psychoService);
    BLE.advertise();
    Serial.println("BLE Master: Advertising for modules");

    for (;;)
    {
        handleMasterBLE();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void handleMasterBLE()
{
    static BLEDevice peripheral;
    static bool wasConnected = false;

    if (!BLE.connected())
    {
        if (wasConnected)
        {
            // Connection lost
            moduleConnectionStatus = false;
            wasConnected = false;
            Serial.println("Disconnected from slave");
            buzzerPlayPredefined(SOUND_DISCONNECT);
        }
        BLE.scanForName("Tenki one");
        peripheral = BLE.available();
        if (peripheral)
        {
            BLE.stopScan();
            if (peripheral.connect())
            {
                psychoCharacteristic.subscribe();
                moduleConnectionStatus = true;
                wasConnected = true;
                Serial.println("Connected to slave");
                buzzerPlayPredefined(SOUND_CONNECT);
            }
        }
    }
    else
    {
        // Check if still connected
        if (!peripheral.connected())
        {
            // Disconnected
            moduleConnectionStatus = false;
            wasConnected = false;
            Serial.println("Disconnected from slave");
            BLE.disconnect(); // Ensure clean disconnect
        }
        else
        {
            // Send CapsLock status
            static bool lastCaps = false;
            if (capsLockStatus != lastCaps)
            {
                uint8_t capsData[1] = {static_cast<uint8_t>(capsLockStatus ? 1 : 0)};
                psychoCharacteristic.writeValue(capsData, 1);
                lastCaps = capsLockStatus;
            }
        }
    }
}

// Common function to handle received keypresses
void handleReceivedKeypress(uint8_t *data, int length)
{
    // Convert BLE data to keypress (to be handled in HostCommunicationBridge)
    HostMessage msg;
    msg.type = KEY_PRESS;
    msg.data = data[0]; // First byte is keycode
    xQueueSend(hostMessageQueue, &msg, 0);
}

void startBleTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(
        BLEHandler,     // Function to be called
        "BLE Handler",  // Name of the task
        stackDepth,     // Stack size in words
        NULL,           // Task input parameter
        priority,       // Priority of the task
        &bleTaskHandle, // Task handle
        core            // Core where the task should run
    );
}