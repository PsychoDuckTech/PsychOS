#include <Arduino.h>
#include <ArduinoBLE.h>
#include "tasks/BLEHandler.h"
#include "globals.h"
#include "utils/initializeBLE.h"
#include "tasks/matrixScan.h"
#include "tasks/buzzer.h"
#include "utils/benchmark.h"

BLEService psychoService(SERVICE_UUID);
BLECharacteristic psychoCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 32);

// Initialize ModuleManager static members
BLEModule ModuleManager::modules[MAX_MODULES] = {};
ModuleManager moduleManager;

// ModuleManager implementation
bool ModuleManager::isModuleAvailable(uint8_t slot) {
    if (slot >= MAX_MODULES) return false;
    return _getModuleRef(slot).state != BLEConnectionState::DISCONNECTED;
}

const char* ModuleManager::getModuleName(uint8_t slot) {
    if (slot >= MAX_MODULES) return "";
    return _getModuleRef(slot).info.name;
}

bool ModuleManager::isModuleConnected(uint8_t slot) {
    if (slot >= MAX_MODULES) return false;
    return _getModuleRef(slot).isConnected;
}

uint8_t ModuleManager::getConnectedModuleCount() {
    uint8_t count = 0;
    for (int i = 0; i < MAX_MODULES; i++) {
        if (_getModuleRef(i).isConnected) count++;
    }
    return count;
}

BLEModule& ModuleManager::getModule(uint8_t slot) {
    return _getModuleRef(slot);
}

int ModuleManager::findFreeSlot() {
    for (int i = 0; i < MAX_MODULES; i++) {
        if (!_getModuleRef(i).isConnected) {
            return i;
        }
    }
    return -1;
}

// Helper function to validate module handshake
bool validateModule(BLEModule& module) {
    BLEMessage request;
    request.type = BLEMessageType::HANDSHAKE_REQUEST;
    strcpy(request.data.handshake.magic, MAGIC_PACKET);
    request.data.handshake.version = 1;
    request.length = sizeof(request.data.handshake);
    
    sendMessageToModule(module, request);
    
    // Wait for response with timeout
    unsigned long startTime = millis();
    while (millis() - startTime < 1000) {
        if (module.characteristic.valueUpdated()) {
            BLEMessage response;
            if (module.characteristic.readValue(response.data.rawData, sizeof(response.data.rawData))) {
                if (response.type == BLEMessageType::HANDSHAKE_RESPONSE &&
                    strcmp((char*)response.data.handshake.magic, MAGIC_RESPONSE) == 0) {
                    return true;
                }
            }
        }
        delay(10);
    }
    return false;
}

void handleReceivedKeypress(uint8_t moduleId, const uint8_t *data, int length) {
    if (!moduleManager.isModuleConnected(moduleId) || length < 2) return;
    
    BLEModule& module = moduleManager.getModule(moduleId);
    uint8_t keyCode = data[0];
    bool isPressed = data[1] == 1;

    // Update active keys tracking for this module
    if (isPressed) {
        if (module.activeKeyCount < 6) {
            module.activeKeys[module.activeKeyCount++] = keyCode;
        }
    } else {
        // Remove key from active keys
        for (int i = 0; i < module.activeKeyCount; i++) {
            if (module.activeKeys[i] == keyCode) {
                // Shift remaining keys left
                for (int j = i; j < module.activeKeyCount - 1; j++) {
                    module.activeKeys[j] = module.activeKeys[j + 1];
                }
                module.activeKeyCount--;
                break;
            }
        }
    }

    // Send to host communication bridge
    HostMessage msg;
    msg.type = isPressed ? KEY_PRESS : KEY_RELEASE;
    msg.data = keyCode;
    xQueueSend(hostMessageQueue, &msg, 0);
}

void sendMessageToModule(BLEModule& module, const BLEMessage& msg) {
    if (module.isConnected && module.characteristic.canWrite()) {
        module.characteristic.writeValue(msg.data.rawData, msg.length);
    }
}

bool handleModuleMessage(BLEModule& module, const BLEMessage& msg) {
    switch (msg.type) {
        case BLEMessageType::KEY_EVENT:
            handleReceivedKeypress(msg.data.keyEvent.moduleId, 
                                 &msg.data.keyEvent.keyCode, 
                                 2);
            return true;
            
        case BLEMessageType::MODULE_INFO:
            memcpy(&module.info, &msg.data.moduleInfo, sizeof(ModuleInfo));
            return true;
            
        case BLEMessageType::HEARTBEAT:
            module.lastHeartbeat = millis();
            module.missedHeartbeats = 0;
            return true;
            
        default:
            return false;
    }
}

void checkModuleConnections() {
    unsigned long currentTime = millis();
    for (int i = 0; i < MAX_MODULES; i++) {
        BLEModule& module = moduleManager.getModule(i);
        if (module.isConnected) {
            // Check if it's time for a heartbeat check
            if (currentTime - module.lastHeartbeat >= MODULE_CHECK_INTERVAL) {
                module.missedHeartbeats++;
                if (module.missedHeartbeats >= MAX_MISSED_CHECKS) {
                    // Disconnect module and release any pressed keys
                    for (int j = 0; j < module.activeKeyCount; j++) {
                        HostMessage msg;
                        msg.type = KEY_RELEASE;
                        msg.data = module.activeKeys[j];
                        xQueueSend(hostMessageQueue, &msg, 0);
                    }
                    module.peripheral.disconnect();
                    module.isConnected = false;
                    module.state = BLEConnectionState::DISCONNECTED;
                    module.activeKeyCount = 0;
                    memset(module.activeKeys, 0, sizeof(module.activeKeys));
                    
                    Serial.print("Module disconnected due to missed heartbeats: ");
                    Serial.println(module.info.name);
                    buzzerPlayPredefined(SOUND_DISCONNECT);
                }
            }
        }
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
    Serial.println("BLE Master: Advertising for peripherals");

    static unsigned long lastModuleCheck = 0;

    for (;;) {
        unsigned long currentTime = millis();

        // Check for new connections from modules
        BLEDevice peripheral = BLE.central();
        if (peripheral) {
            Serial.println("Connection attempt from peripheral");
            
            int slot = ModuleManager::findFreeSlot();
            if (slot >= 0) {
                BLEModule& module = moduleManager.getModule(slot);
                module.peripheral = peripheral;
                
                if (peripheral.discoverAttributes()) {
                    BLEService service = peripheral.service(SERVICE_UUID);
                    if (service) {
                        module.characteristic = service.characteristic(CHARACTERISTIC_UUID);
                        if (module.characteristic && module.characteristic.subscribe()) {
                            // Validate that this is a compatible module
                            if (validateModule(module)) {
                                // Request module information
                                BLEMessage infoRequest;
                                infoRequest.type = BLEMessageType::MODULE_INFO;
                                infoRequest.length = 0;
                                sendMessageToModule(module, infoRequest);
                                
                                module.state = BLEConnectionState::CONNECTED;
                                module.isConnected = true;
                                module.lastHeartbeat = currentTime;
                                module.missedHeartbeats = 0;
                                
                                Serial.print("Module connected in slot ");
                                Serial.println(slot);
                                buzzerPlayPredefined(SOUND_CONNECT);
                            } else {
                                Serial.println("Module validation failed");
                                peripheral.disconnect();
                            }
                        }
                    }
                }
            } else {
                Serial.println("No available module slots");
                peripheral.disconnect();
            }
        }

        // Handle messages from connected modules
        for (int i = 0; i < MAX_MODULES; i++) {
            BLEModule& module = moduleManager.getModule(i);
            if (module.isConnected && module.characteristic.valueUpdated()) {
                BLEMessage msg;
                if (module.characteristic.readValue(msg.data.rawData, sizeof(msg.data.rawData))) {
                    handleModuleMessage(module, msg);
                }
            }
        }

        // Periodic connection check
        if (currentTime - lastModuleCheck >= MODULE_CHECK_INTERVAL) {
            checkModuleConnections();
            lastModuleCheck = currentTime;
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