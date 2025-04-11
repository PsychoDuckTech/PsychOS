#pragma once

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "main.h"

#define MAX_MODULES 3
#define MODULE_CHECK_INTERVAL 60
#define MAX_MISSED_CHECKS 3
#define MAGIC_PACKET "PSYCHO_KB"
#define MAGIC_RESPONSE "PMOD"

// Message types for BLE communication
enum class BLEMessageType {
    KEY_EVENT,
    CAPS_LOCK_STATUS,
    RGB_CONTROL,
    SYSTEM_STATUS,
    HANDSHAKE_REQUEST,
    HANDSHAKE_RESPONSE,
    HEARTBEAT,
    MODULE_INFO,
    CUSTOM_COMMAND
};

// Connection states
enum class BLEConnectionState {
    DISCONNECTED,
    AUTHENTICATING,
    REQUESTING_INFO,
    CONNECTED,
    ERROR
};

struct ModuleInfo {
    char name[32];
    uint8_t moduleType;
    uint8_t features;
    uint8_t version;
};

struct BLEModule {
    BLEDevice peripheral;
    BLECharacteristic characteristic;
    BLEConnectionState state;
    ModuleInfo info;
    unsigned long lastHeartbeat;
    uint8_t missedHeartbeats;
    bool isConnected;
    uint8_t activeKeys[6];
    uint8_t activeKeyCount;
};

// Message structure for BLE communication
struct BLEMessage {
    BLEMessageType type;
    union {
        struct {
            uint8_t moduleId;
            uint8_t keyCode;
            bool isPressed;
        } keyEvent;
        struct {
            bool enabled;
        } capsLock;
        struct {
            char magic[8];
            uint8_t version;
        } handshake;
        ModuleInfo moduleInfo;
        uint8_t rawData[32];  // Increased max size for larger messages
    } data;
    uint8_t length;
};

// Module Management Class
class ModuleManager {
public:
    static bool isModuleAvailable(uint8_t slot);
    static const char* getModuleName(uint8_t slot);
    static bool isModuleConnected(uint8_t slot);
    static uint8_t getConnectedModuleCount();
    static BLEModule& getModule(uint8_t slot);
    
protected:
    static BLEModule& _getModuleRef(uint8_t slot) { return modules[slot]; }
    static int findFreeSlot();
    
private:
    static BLEModule modules[MAX_MODULES];
    friend void BLEHandler(void *parameter);
    friend void checkModuleConnections();
};

extern ModuleManager moduleManager;

// Function declarations
bool validateModule(BLEModule& module);
bool handleModuleMessage(BLEModule& module, const BLEMessage& msg);
void sendMessageToModule(BLEModule& module, const BLEMessage& msg);
void handleReceivedKeypress(uint8_t moduleId, const uint8_t *data, int length);
void startBleTask(UBaseType_t core = 1, uint32_t stackDepth = 16384, UBaseType_t priority = 1);

// BLE Service and Characteristic UUIDs
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-000000000001"

// BLE Service and Characteristic
extern BLEService psychoService;
extern BLECharacteristic psychoCharacteristic;