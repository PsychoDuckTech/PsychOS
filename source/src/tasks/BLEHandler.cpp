#include <Arduino.h>
#include <ArduinoBLE.h>
#include "tasks/BLEHandler.h"
#include "globals.h"
#include "utils/initializeBLE.h"
#include "tasks/matrixScan.h"
#include "tasks/buzzer.h"
#include "utils/benchmark.h"

BLEService psychoService(SERVICE_UUID);
BLECharacteristic psychoCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 20);

static BLEConnection connection;
static uint8_t activeModuleKeys[6] = {0};
static int activeKeyCount = 0;

// Helper function to handle disconnection cleanup
static void handleDisconnection() {
    if (connection.isConnected) {
        // Release any stuck keys
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
        
        Serial.println("Disconnected from slave");
        BLE.disconnect();
        BLE.stopScan();
        BLE.advertise();
        buzzerPlayPredefined(SOUND_DISCONNECT);
    }
}

// Handle the connection state machine
bool handleConnection(BLEConnection& conn) {
    switch (conn.state) {
        case BLEConnectionState::DISCONNECTED:
            BLE.scanForName("Tenki one");
            conn.peripheral = BLE.available();
            if (conn.peripheral) {
                BLE.stopScan();
                conn.state = BLEConnectionState::CONNECTING;
            }
            break;

        case BLEConnectionState::CONNECTING:
            if (conn.peripheral.connect()) {
                conn.state = BLEConnectionState::DISCOVERING_SERVICES;
            } else {
                conn.state = BLEConnectionState::DISCONNECTED;
            }
            break;

        case BLEConnectionState::DISCOVERING_SERVICES:
            if (conn.peripheral.discoverAttributes()) {
                BLEService service = conn.peripheral.service(SERVICE_UUID);
                if (service) {
                    conn.characteristic = service.characteristic(CHARACTERISTIC_UUID);
                    if (conn.characteristic && conn.characteristic.subscribe()) {
                        conn.state = BLEConnectionState::CONNECTED;
                        conn.isConnected = true;
                        moduleConnectionStatus = true;
                        Serial.println("Connected to slave and subscribed to characteristic");
                        buzzerPlayPredefined(SOUND_CONNECT);
                        return true;
                    }
                }
                Serial.println("Failed to setup BLE characteristic");
            }
            conn.peripheral.disconnect();
            conn.state = BLEConnectionState::DISCONNECTED;
            break;

        case BLEConnectionState::CONNECTED:
            if (!BLE.connected() || !conn.peripheral.connected()) {
                handleDisconnection();
                return false;
            }
            return true;
    }
    return false;
}

// Handle received BLE messages
bool handleMessageReceived(const BLEMessage& msg) {
    switch (msg.type) {
        case BLEMessageType::KEY_EVENT:
            handleReceivedKeypress(msg.data.rawData, msg.length);
            return true;
            
        case BLEMessageType::CAPS_LOCK_STATUS:
            // Handle caps lock status updates
            return true;
            
        default:
            return false;
    }
}

void handleReceivedKeypress(const uint8_t *data, int length) {
    if (length < 2) return;

    uint8_t keyCode = data[0];
    bool isPressed = data[1] == 1;

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

    // Send to host communication bridge
    xQueueSend(hostMessageQueue, &msg, 0);
}

// Send a message over BLE
void sendMessage(BLEConnection& conn, const BLEMessage& msg) {
    if (conn.isConnected && conn.characteristic.canWrite()) {
        conn.characteristic.writeValue(msg.data.rawData, msg.length);
    }
}

void BLEHandler(void *parameter) {
    // Initialize BLE
    initializeBLE();
    BLE.setLocalName("Kibodo one");
    BLE.setDeviceName("Kibodo one");
    psychoService.addCharacteristic(psychoCharacteristic);
    BLE.addService(psychoService);
    BLE.advertise();
    Serial.println("BLE Master: Advertising for modules");

    // Initialize connection state
    connection.state = BLEConnectionState::DISCONNECTED;
    connection.isConnected = false;
    connection.lastActivityTime = 0;

    static unsigned long lastKeyTime = 0;
    static bool lastCaps = false;

    for (;;) {
        unsigned long currentTime = millis();

        // Handle connection state machine
        if (!handleConnection(connection)) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            continue;
        }

        // Process incoming data
        BLE.poll();
        if (connection.characteristic && connection.characteristic.valueUpdated()) {
            if (currentTime - lastKeyTime > 20) {
                uint8_t data[2];
                if (connection.characteristic.readValue(data, 2)) {
                    BLEMessage msg;
                    msg.type = BLEMessageType::KEY_EVENT;
                    memcpy(msg.data.rawData, data, 2);
                    msg.length = 2;
                    handleMessageReceived(msg);
                    lastKeyTime = currentTime;
                }
            }
        }

        // Handle caps lock status updates
        if (capsLockStatus != lastCaps) {
            BLEMessage msg;
            msg.type = BLEMessageType::CAPS_LOCK_STATUS;
            msg.data.capsLock.enabled = capsLockStatus;
            msg.length = 1;
            sendMessage(connection, msg);
            lastCaps = capsLockStatus;
            Serial.println("Sent CapsLock status to module");
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void startBleTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority) {
    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(
        BLEHandler,
        "BLE Handler",
        stackDepth,
        NULL,
        priority,
        &bleTaskHandle,
        core);
}