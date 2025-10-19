#pragma once

#include "arduino_compat.h"
#include "tinyusb.h"
#include "tusb_hid.h"

// HID Key Codes
#define KEY_LEFT_CTRL   0xE0
#define KEY_LEFT_SHIFT  0xE1
#define KEY_LEFT_ALT    0xE2
#define KEY_LEFT_GUI    0xE4
#define KEY_RIGHT_CTRL  0xE4
#define KEY_RIGHT_SHIFT 0xE5
#define KEY_RIGHT_ALT   0xE6
#define KEY_RIGHT_GUI   0xE7

#define KEY_ESC         0x29
#define KEY_F1          0x3A
#define KEY_F2          0x3B
#define KEY_F3          0x3C
#define KEY_F4          0x3D
#define KEY_F5          0x3E
#define KEY_F6          0x3F
#define KEY_F7          0x40
#define KEY_F8          0x41
#define KEY_F9          0x42
#define KEY_F10         0x43
#define KEY_F11         0x44
#define KEY_F12         0x45

#define KEY_CAPS_LOCK   0x39
#define KEY_INSERT      0x49
#define KEY_DELETE      0x4C

#define HID_KEY_SYSREQ_ATTENTION 0x46
#define HID_KEY_ENTER   0x28
#define HID_KEY_BACKSPACE 0x2A

// Consumer Control codes
#define CONSUMER_CONTROL_VOLUME_INCREMENT 0xE9
#define CONSUMER_CONTROL_VOLUME_DECREMENT 0xEA
#define CONSUMER_CONTROL_MUTE 0xE2
#define CONSUMER_CONTROL_PLAY_PAUSE 0xCD
#define CONSUMER_CONTROL_SCAN_NEXT 0xB5
#define CONSUMER_CONTROL_SCAN_PREVIOUS 0xB6

// Navigation keys
#define HID_KEY_UP_ARROW    0x52
#define HID_KEY_DOWN_ARROW  0x51
#define HID_KEY_LEFT_ARROW  0x50
#define HID_KEY_RIGHT_ARROW 0x4F
#define HID_KEY_PAGE_UP     0x4B
#define HID_KEY_PAGE_DOWN   0x4E
#define HID_KEY_HOME        0x4A
#define HID_KEY_END         0x4D

// Arrow key aliases
#define D4 HID_KEY_UP_ARROW
#define D9 HID_KEY_DOWN_ARROW
#define D7 HID_KEY_LEFT_ARROW
#define D6 HID_KEY_RIGHT_ARROW

class USBHIDKeyboard {
private:
    uint8_t _keyReport[8];
    bool _initialized;
    
public:
    USBHIDKeyboard() : _initialized(false) {
        memset(_keyReport, 0, sizeof(_keyReport));
    }
    
    void begin() {
        if (!_initialized) {
            _initialized = true;
        }
    }
    
    void end() {
        releaseAll();
        _initialized = false;
    }
    
    void press(uint8_t key) {
        if (!_initialized) return;
        
        // Handle modifier keys
        if (key >= 0xE0 && key <= 0xE7) {
            _keyReport[0] |= (1 << (key - 0xE0));
        } else {
            // Find empty slot for regular key
            for (int i = 2; i < 8; i++) {
                if (_keyReport[i] == 0) {
                    _keyReport[i] = key;
                    break;
                }
            }
        }
        sendReport();
    }
    
    void release(uint8_t key) {
        if (!_initialized) return;
        
        // Handle modifier keys
        if (key >= 0xE0 && key <= 0xE7) {
            _keyReport[0] &= ~(1 << (key - 0xE0));
        } else {
            // Find and remove regular key
            for (int i = 2; i < 8; i++) {
                if (_keyReport[i] == key) {
                    _keyReport[i] = 0;
                }
            }
        }
        sendReport();
    }
    
    void releaseAll() {
        memset(_keyReport, 0, sizeof(_keyReport));
        sendReport();
    }
    
    void write(uint8_t key) {
        press(key);
        delay(10);
        release(key);
    }
    
private:
    void sendReport() {
        if (_initialized && tud_hid_ready()) {
            tud_hid_keyboard_report(0, _keyReport[0], &_keyReport[2]);
        }
    }
};

class USBHIDConsumerControl {
private:
    bool _initialized;
    
public:
    USBHIDConsumerControl() : _initialized(false) {}
    
    void begin() {
        if (!_initialized) {
            _initialized = true;
        }
    }
    
    void end() {
        _initialized = false;
    }
    
    void press(uint16_t usage) {
        if (_initialized && tud_hid_ready()) {
            tud_hid_report(1, &usage, sizeof(usage));
        }
    }
    
    void release() {
        if (_initialized && tud_hid_ready()) {
            uint16_t empty = 0;
            tud_hid_report(1, &empty, sizeof(empty));
        }
    }
    
    void write(uint16_t usage) {
        press(usage);
        delay(10);
        release();
    }
};
