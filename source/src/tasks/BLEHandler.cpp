#include <Arduino.h>
#include <ArduinoBLE.h>
#include "tasks/BLEHandler.h"
#include "globals.h"
#include "utils/initializeBLE.h"
#include "tasks/matrixScan.h"
#include "tasks/buzzer.h"
#include "utils/benchmark.h"

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
    static unsigned long lastKeyTime = 0;

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
                if (currentTime - lastKeyTime > 20)
                {
                    uint8_t data[2];
                    if (moduleChar.readValue(data, 2))
                    {
                        uint8_t keyCode = data[0];
                        bool isPressed = data[1] == 1;
                        // transmission debugging
                        // Serial.print("Received key event: code=");
                        // Serial.print(keyCode);
                        // Serial.print(", state=");
                        // Serial.println(isPressed ? "pressed" : "released");

                        handleReceivedKeypress(data, 2);

                        lastKeyTime = currentTime;
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
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void handleReceivedKeypress(uint8_t *data, int length)
{
    uint8_t keyCode = data[0];
    bool isPressed = data[1] == 1;

    // Conversion table for number pad keys
    switch (keyCode)
    {
    case 0x1E:
        keyCode = 0x1E;
        break;
    case 0x1F:
        keyCode = 0x1F;
        break;
    case 0x20:
        keyCode = 0x20;
        break;
    case 0x21:
        keyCode = 0x21;
        break;
    case 0x27:
        keyCode = 0x27;
        break;
    // Numpad keys
    case 0x53: // Numlock
        keyCode = 0x53;
        break;
    case 0x54: // Keypad /
        keyCode = 0x54;
        break;
    case 0x55: // Keypad *
        keyCode = 0x55;
        break;
    case 0x56: // Keypad -
        keyCode = 0x56;
        break;
    case 0x57: // Keypad +
        keyCode = 0x57;
        break;
    case 0x58: // Keypad Enter
        keyCode = 0x58;
        break;
    case 0x59: // Keypad 1 / End
        keyCode = 0x1E;
        break;
    case 0x5A: // Keypad 2 / Down
        keyCode = 0x1F;
        break;
    case 0x5B: // Keypad 3 / Page Down
        keyCode = 0x20;
        break;
    case 0x5C: // Keypad 4 / Left
        keyCode = 0x21;
        break;
    case 0x5D: // Keypad 5
        keyCode = 0x22;
        break;
    case 0x5E: // Keypad 6 / Right
        keyCode = 0x23;
        break;
    case 0x5F: // Keypad 7 / Home
        keyCode = 0x24;
        break;
    case 0x60: // Keypad 8 / Up
        keyCode = 0x25;
        break;
    case 0x61: // Keypad 9 / Page Up
        keyCode = 0x26;
        break;
    case 0x62: // Keypad 0 / Insert
        keyCode = 0x27;
        break;
    case 0x63: // Keypad . / Delete
        keyCode = 0x27;
        break;
    default:
        break;
    }

    HostMessage msg;
    msg.type = isPressed ? KEY_PRESS : KEY_RELEASE;
    msg.data = keyCode;

    xQueueSend(hostMessageQueue, &msg, 0);

    // Serial.print("Handled HID key event: code=");
    // Serial.print(keyCode);
    // Serial.print(", state=");
    // Serial.println(isPressed ? "pressed" : "released");
}

void startBleTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(
        BLEHandler, "BLE Handler", 20480, NULL, priority, &bleTaskHandle, core);
}