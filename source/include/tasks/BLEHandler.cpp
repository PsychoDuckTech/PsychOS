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
    BLE.setLocalName("Kibodo one");
    BLE.setDeviceName("Kibodo one");
    psychoService.addCharacteristic(psychoCharacteristic);
    BLE.addService(psychoService);
    BLE.advertise();
    Serial.println("BLE Master: Advertising for modules");

    static BLEDevice peripheral;
    static BLECharacteristic moduleChar;
    static bool wasConnected = false;
    static uint8_t activeModuleKeys[6] = {0};
    static int activeKeyCount = 0;
    static unsigned long lastKeyTime = 0; // Debounce for rapid keys

    for (;;)
    {
        unsigned long currentTime = millis();

        if (!BLE.connected() || !peripheral.connected())
        {
            if (wasConnected)
            {
                for (int i = 0; i < activeKeyCount; i++)
                {
                    HostMessage msg;
                    msg.type = KEY_RELEASE;
                    msg.data = activeModuleKeys[i];
                    xQueueSend(hostMessageQueue, &msg, 0);
                    Serial.print("Released stuck key on disconnect: ");
                    Serial.println(activeModuleKeys[i]);
                }
                activeKeyCount = 0;
                memset(activeModuleKeys, 0, sizeof(activeModuleKeys));

                moduleConnectionStatus = false;
                wasConnected = false;
                Serial.println("Disconnected from slave");
                peripheral = BLEDevice();
                moduleChar = BLECharacteristic();
                BLE.disconnect();
                BLE.stopScan();
                BLE.advertise();
                buzzerPlayPredefined(SOUND_DISCONNECT);
            }

            BLE.scanForName("Tenki one");
            peripheral = BLE.available();
            if (peripheral)
            {
                BLE.stopScan();
                if (peripheral.connect())
                {
                    if (peripheral.discoverAttributes())
                    {
                        BLEService service = peripheral.service(SERVICE_UUID);
                        if (service)
                        {
                            moduleChar = service.characteristic(CHARACTERISTIC_UUID);
                            if (moduleChar)
                            {
                                if (moduleChar.subscribe())
                                {
                                    moduleConnectionStatus = true;
                                    wasConnected = true;
                                    Serial.println("Connected to slave and subscribed to characteristic");
                                    buzzerPlayPredefined(SOUND_CONNECT);
                                }
                                else
                                {
                                    Serial.println("Failed to subscribe to characteristic");
                                    peripheral.disconnect();
                                }
                            }
                            else
                            {
                                Serial.println("Characteristic not found");
                                peripheral.disconnect();
                            }
                        }
                        else
                        {
                            Serial.println("Service not found");
                            peripheral.disconnect();
                        }
                    }
                    else
                    {
                        Serial.println("Failed to discover attributes");
                        peripheral.disconnect();
                    }
                }
            }
        }
        else
        {
            BLE.poll();
            if (moduleChar && moduleChar.valueUpdated())
            {
                // Rate limit to prevent overflow
                if (currentTime - lastKeyTime > 20)
                { // 20ms debounce
                    uint8_t data[2];
                    if (moduleChar.readValue(data, 2))
                    {
                        uint8_t keyCode = data[0]; // HID usage ID from BLE
                        bool isPressed = data[1] == 1;

                        HostMessage msg;
                        msg.type = isPressed ? KEY_PRESS : KEY_RELEASE;
                        msg.data = keyCode; // Directly pass HID code
                        xQueueSend(hostMessageQueue, &msg, 0);
                    }
                }
            }

            static bool lastCaps = false;
            if (capsLockStatus != lastCaps && moduleChar.canWrite())
            {
                uint8_t capsData[1] = {static_cast<uint8_t>(capsLockStatus ? 1 : 0)};
                moduleChar.writeValue(capsData, 1);
                lastCaps = capsLockStatus;
                Serial.println("Sent CapsLock status to module");
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Reverted to 10ms
    }
}

void startBleTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(
        BLEHandler, "BLE Handler", 20480, NULL, priority, &bleTaskHandle, core); // Increased stack size
}