#pragma once

#include "arduino_compat.h"
#include <string>

// BLE compatibility layer for ESP-IDF
// Using ESP-IDF's Bluedroid/NimBLE stack

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

// String class for compatibility
class String {
private:
    char* _buffer;
    size_t _length;
    
public:
    String() : _buffer(nullptr), _length(0) {}
    
    String(const char* str) : _buffer(nullptr), _length(0) {
        if (str) {
            _length = strlen(str);
            _buffer = (char*)malloc(_length + 1);
            if (_buffer) {
                strcpy(_buffer, str);
            }
        }
    }
    
    String(const String& other) : _buffer(nullptr), _length(0) {
        if (other._buffer) {
            _length = other._length;
            _buffer = (char*)malloc(_length + 1);
            if (_buffer) {
                strcpy(_buffer, other._buffer);
            }
        }
    }
    
    ~String() {
        if (_buffer) {
            free(_buffer);
        }
    }
    
    String& operator=(const String& other) {
        if (this != &other) {
            if (_buffer) {
                free(_buffer);
            }
            if (other._buffer) {
                _length = other._length;
                _buffer = (char*)malloc(_length + 1);
                if (_buffer) {
                    strcpy(_buffer, other._buffer);
                }
            } else {
                _buffer = nullptr;
                _length = 0;
            }
        }
        return *this;
    }
    
    String& operator=(const char* str) {
        if (_buffer) {
            free(_buffer);
        }
        if (str) {
            _length = strlen(str);
            _buffer = (char*)malloc(_length + 1);
            if (_buffer) {
                strcpy(_buffer, str);
            }
        } else {
            _buffer = nullptr;
            _length = 0;
        }
        return *this;
    }
    
    bool operator==(const String& other) const {
        if (!_buffer && !other._buffer) return true;
        if (!_buffer || !other._buffer) return false;
        return strcmp(_buffer, other._buffer) == 0;
    }
    
    const char* c_str() const {
        return _buffer ? _buffer : "";
    }
    
    size_t length() const {
        return _length;
    }
};

// BLE property flags
#define BLERead      0x02
#define BLEWrite     0x08
#define BLENotify    0x10

// Forward declarations
class BLECharacteristic;
class BLEService;
class BLEDevice;

class BLECharacteristic {
private:
    const char* _uuid;
    uint8_t _properties;
    size_t _valueSize;
    uint8_t _value[20];
    
public:
    BLECharacteristic(const char* uuid, uint8_t properties, size_t valueSize)
        : _uuid(uuid), _properties(properties), _valueSize(valueSize) {
        memset(_value, 0, sizeof(_value));
    }
    
    void writeValue(const uint8_t* value, size_t length) {
        if (length > sizeof(_value)) length = sizeof(_value);
        memcpy(_value, value, length);
        _valueSize = length;
    }
    
    size_t valueLength() const {
        return _valueSize;
    }
    
    const uint8_t* value() const {
        return _value;
    }
    
    const char* uuid() const {
        return _uuid;
    }
};

class BLEService {
private:
    const char* _uuid;
    
public:
    BLEService(const char* uuid) : _uuid(uuid) {}
    
    const char* uuid() const {
        return _uuid;
    }
};

class BLEDevice {
private:
    String _address;
    int _rssi;
    bool _connected;
    
public:
    BLEDevice() : _rssi(0), _connected(false) {}
    
    String address() const {
        return _address;
    }
    
    int rssi() const {
        return _rssi;
    }
    
    bool connected() const {
        return _connected;
    }
    
    void setAddress(const char* addr) {
        _address = addr;
    }
    
    void setRssi(int rssi) {
        _rssi = rssi;
    }
    
    void setConnected(bool connected) {
        _connected = connected;
    }
};

class BLEClass {
private:
    bool _initialized;
    
public:
    BLEClass() : _initialized(false) {}
    
    bool begin() {
        if (_initialized) return true;
        
        // Initialize BLE controller
        esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
        esp_err_t ret = esp_bt_controller_init(&bt_cfg);
        if (ret != ESP_OK) return false;
        
        ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
        if (ret != ESP_OK) return false;
        
        ret = esp_bluedroid_init();
        if (ret != ESP_OK) return false;
        
        ret = esp_bluedroid_enable();
        if (ret != ESP_OK) return false;
        
        _initialized = true;
        return true;
    }
    
    void end() {
        if (_initialized) {
            esp_bluedroid_disable();
            esp_bluedroid_deinit();
            esp_bt_controller_disable();
            esp_bt_controller_deinit();
            _initialized = false;
        }
    }
    
    void advertise() {
        // Stub - implement advertising
    }
    
    void stopScan() {
        // Stub - implement scan stop
    }
    
    void disconnect() {
        // Stub - implement disconnect
    }
    
    void setDeviceName(const char* name) {
        esp_ble_gap_set_device_name(name);
    }
};

extern BLEClass BLE;
