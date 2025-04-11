#pragma once

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "main.h"

// Message types for BLE communication
enum class BLEMessageType {
    KEY_EVENT,
    CAPS_LOCK_STATUS,
    RGB_CONTROL,
    SYSTEM_STATUS,
    CUSTOM_COMMAND
};

// Connection states
enum class BLEConnectionState {
    DISCONNECTED,
    SCANNING,
    CONNECTING,
    DISCOVERING_SERVICES,
    CONNECTED
};

struct BLEConnection {
    BLEDevice peripheral;
    BLECharacteristic characteristic;
    BLEConnectionState state;
    unsigned long lastActivityTime;
    bool isConnected;
};

// Message structure for BLE communication
struct BLEMessage {
    BLEMessageType type;
    union {
        struct {
            uint8_t keyCode;
            bool isPressed;
        } keyEvent;
        struct {
            bool enabled;
        } capsLock;
        uint8_t rawData[20];  // Max size for other message types
    } data;
    uint8_t length;
};

// Function declarations
void BLEHandler(void *parameter);
bool handleConnection(BLEConnection& conn);
bool handleMessageReceived(const BLEMessage& msg);
void sendMessage(BLEConnection& conn, const BLEMessage& msg);
void handleReceivedKeypress(const uint8_t *data, int length);
void startBleTask(UBaseType_t core = 1, uint32_t stackDepth = 16384, UBaseType_t priority = 1);

// BLE Service and Characteristic UUIDs
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-000000000001"

// BLE Service and Characteristic
extern BLEService psychoService;
extern BLECharacteristic psychoCharacteristic;