#include <Arduino.h>
#include <ArduinoBLE.h>
#include "BLEHandler.h"
#include "globals.h"
#include "utils/initializeBLE.h"
#include "tasks/matrixScan.h"

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

#ifdef BLE_MASTER // MASTER SETUP
    BLE.setLocalName("PsychoMaster");
    BLE.setDeviceName("PsychoMaster");
    psychoService.addCharacteristic(psychoCharacteristic);
    BLE.addService(psychoService);
    BLE.advertise();
    Serial.println("BLE Master: Advertising for modules");
#elif defined(BLE_SLAVE) // SLAVE SETUP
    BLE.setLocalName("PsychoSlave");
    BLE.setDeviceName("PsychoSlave");
#endif

    for (;;)
    {
#ifdef BLE_MASTER
        handleMasterBLE();
#elif defined(BLE_SLAVE)
        handleSlaveBLE();
#endif
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// MASTER-SPECIFIC HANDLING
#ifdef BLE_MASTER
void handleMasterBLE()
{
    static BLEDevice peripheral;

    if (!BLE.connected())
    {
        BLE.scanForName("PsychoNumpad"); // Target module name
        peripheral = BLE.available();
        if (peripheral)
        {
            BLE.stopScan();
            peripheral.connect();
            psychoCharacteristic.subscribe();
            moduleConnectionStatus = true;
        }
    }

    // Send CapsLock status to module
    static bool lastCaps = false;
    if (capsLockStatus != lastCaps)
    {
        uint8_t capsData[1] = {capsLockStatus ? 1 : 0};
        psychoCharacteristic.writeValue(capsData, 1);
        lastCaps = capsLockStatus;
    }
}
#endif

// SLAVE-SPECIFIC HANDLING
#ifdef BLE_SLAVE
void handleSlaveBLE()
{
    static bool autoConnected = false;

    if (!autoConnected)
    {
        BLEDevice central = BLE.central();
        if (central && central.localName() == "PsychoMaster")
        {
            Serial.println("Connected to master!");
            moduleConnectionStatus = true;
            autoConnected = true;
        }
    }

    if (moduleConnectionStatus)
    {
        // Keypress data will be sent through matrixScan
    }
}
#endif

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