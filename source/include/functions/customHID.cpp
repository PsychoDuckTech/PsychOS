#include "customHID.h"
#include "tusb.h"

// Define the global Keyboard instance
customHID Keyboard;

static const uint8_t HID_REPORT_DESCRIPTOR[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection (Application)
    0x85, 0x01,  // Report ID (1)
    
    // Modifiers
    0x05, 0x07,  // Usage Page (Key Codes)
    0x19, 0xE0,  // Usage Minimum (224)
    0x29, 0xE7,  // Usage Maximum (231)
    0x15, 0x00,  // Logical Minimum (0)
    0x25, 0x01,  // Logical Maximum (1)
    0x75, 0x01,  // Report Size (1)
    0x95, 0x08,  // Report Count (8)
    0x81, 0x02,  // Input (Data, Variable, Absolute)
    
    // Reserved byte
    0x95, 0x01,  // Report Count (1)
    0x75, 0x08,  // Report Size (8)
    0x81, 0x01,  // Input (Constant)
    
    // LED states
    0x95, 0x05,  // Report Count (5)
    0x75, 0x01,  // Report Size (1)
    0x05, 0x08,  // Usage Page (LEDs)
    0x19, 0x01,  // Usage Minimum (1)
    0x29, 0x05,  // Usage Maximum (5)
    0x91, 0x02,  // Output (Data, Variable, Absolute)
    
    // LED padding
    0x95, 0x01,  // Report Count (1)
    0x75, 0x03,  // Report Size (3)
    0x91, 0x01,  // Output (Constant)
    
    // Regular keys
    0x95, 0x06,  // Report Count (6)
    0x75, 0x08,  // Report Size (8)
    0x15, 0x00,  // Logical Minimum (0)
    0x25, 0x65,  // Logical Maximum (101)
    0x05, 0x07,  // Usage Page (Key Codes)
    0x19, 0x00,  // Usage Minimum (0)
    0x29, 0x65,  // Usage Maximum (101)
    0x81, 0x00,  // Input (Data, Array)
    
    0xC0         // End Collection
};

void customHID::begin() {
    tusb_init();
}

void customHID::sendReport(matrixReport* report) {
    if (tud_hid_ready()) {
        tud_hid_keyboard_report(REPORT_ID, 
            report->modifiers,
            report->keys);
    }
}

void customHID::releaseAll() {
    memset(&_report, 0, sizeof(matrixReport));
    sendReport(&_report);
}

// TinyUSB callbacks
extern "C" {
    uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
        return 0;
    }

    void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    }
}