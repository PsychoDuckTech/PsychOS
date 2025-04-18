#include <Arduino.h>
#include <ArduinoBLE.h>
#include "tasks/BLEHandler.h"
#include "globals.h"
#include "utils/initializeBLE.h"
#include "tasks/matrixScan.h"
#include "tasks/buzzer.h"
#include "utils/benchmark.h"

// Add static buffer for device name
static char deviceNameBuffer[32];

BLEService psychoService(SERVICE_UUID);
BLECharacteristic psychoCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 20);

static BLEConnection connection;
static uint8_t activeModuleKeys[6] = {0};
static int activeKeyCount = 0;

// Initialize module statistics
ModuleStat moduleStats[4];
int numModules = 0;

// Helper function to handle disconnection cleanup
static void handleDisconnection() {
    // Release any stuck keys regardless of connection state
    for (int i = 0; i < activeKeyCount; i++) {
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
    connection.isConnected = false;
    connection.state = BLEConnectionState::DISCONNECTED;
    deviceNameBuffer[0] = '\0';
    connectedModuleName = deviceNameBuffer;

    Serial.print("Disconnected from module: ");
    Serial.println(deviceNameBuffer[0] != '\0' ? deviceNameBuffer : "Unknown device");
    Serial.println("Restarting BLE advertising...");
    BLE.disconnect();
    BLE.stopScan();
    BLE.advertise();
}

void handleReceivedKeypress(const uint8_t *data, int length) {
    if (length < 2)
        return;

    uint8_t keyCode = data[0];
    bool isPressed = data[1] == 1;

    // Update key press statistics for the connected module
    String address = connection.peripheral.address();
    for (int i = 0; i < numModules; i++) {
        if (moduleStats[i].address == address && isPressed) {
            moduleStats[i].keyPresses++;
            moduleStats[i].rssi = connection.peripheral.rssi(); // Update RSSI on key press
            break;
        }
    }

    // Update active keys tracking
    if (isPressed) {
        if (activeKeyCount < 6) {
            activeModuleKeys[activeKeyCount++] = keyCode;
        }
    } else {
        // Remove key from active keys
        for (int i = 0; i < activeKeyCount; i++) {
            if (activeModuleKeys[i] == keyCode) {
                // Shift remaining keys left
                for (int j = i; j < activeKeyCount - 1; j++) {
                    activeModuleKeys[j] = activeModuleKeys[j + 1];
                }
                activeKeyCount--;
                break;
            }
        }
    }

    // Convert keycode to host message
    HostMessage msg;
    msg.type = isPressed ? KEY_PRESS : KEY_RELEASE;
    msg.data = keyCode;
    xQueueSend(hostMessageQueue, &msg, 0);
}

void BLEHandler(void *parameter) {
    Serial.println("Starting BLE Handler...");
    initializeBLE();
    BLE.setLocalName("Kibodo One");
    psychoService.addCharacteristic(psychoCharacteristic);
    BLE.addService(psychoService);
    BLE.advertise();
    Serial.println("BLE advertising started as 'Kibodo One'");

    for (;;) {
        BLEDevice central = BLE.central();
        if (central) {
            if (central.connected()) {
                if (central.discoverAttributes()) {
                    moduleConnectionStatus = true;
                    // Update our connection state
                    connection.peripheral = central;
                    String name = central.localName();

                    // Try to get the name a few times if needed
                    int retries = 3;
                    while (name.length() == 0 && retries > 0) {
                        delay(50);
                        name = central.localName();
                        retries--;
                    }

                    String address = central.address();
                    bool found = false;
                    for (int i = 0; i < numModules; i++) {
                        if (moduleStats[i].address == address) {
                            found = true;
                            moduleStats[i].connectTime = millis();
                            moduleStats[i].rssi = central.rssi();
                            break;
                        }
                    }
                    
                    if (!found && numModules < 4) {
                        moduleStats[numModules].address = address;
                        moduleStats[numModules].keyPresses = 0;
                        moduleStats[numModules].connectTime = millis();
                        moduleStats[numModules].rssi = central.rssi();
                        numModules++;
                    }

                    if (name.length() > 0) {
                        strncpy(deviceNameBuffer, name.c_str(), sizeof(deviceNameBuffer) - 1);
                        deviceNameBuffer[sizeof(deviceNameBuffer) - 1] = '\0';
                        connectedModuleName = deviceNameBuffer;
                        Serial.print("Connected to module: ");
                        Serial.println(deviceNameBuffer);
                    } else {
                        Serial.println("Warning: Could not get device name after multiple attempts");
                    }
                    Serial.print("Address: ");
                    Serial.println(central.address());

                    while (central.connected() && BLE.connected()) {
                        if (psychoCharacteristic.written()) {
                            uint8_t data[2];
                            int length = psychoCharacteristic.readValue(data, 2);
                            handleReceivedKeypress(data, length);
                        }
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                    }
                    
                    Serial.println("Connection lost, cleaning up...");
                    handleDisconnection();
                    moduleConnectionStatus = false;
                }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void startBleTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority) {
    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(
        BLEHandler, "BLE Handler", stackDepth, NULL, priority, &bleTaskHandle, core);
}