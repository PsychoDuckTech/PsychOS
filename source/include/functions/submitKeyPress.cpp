#include "submitKeyPress.h"
#include "../lib/usb_hid_keys/usb_hid_keys.h"

void submitKeyPress(uint8_t key, bool pressed) {
    static uint8_t hidReport[6] = {0};  // 6-key rollover
    bool modified = false;

    if (pressed) {
        // Add key to report
        for (int i = 0; i < 6; i++) {
            if (hidReport[i] == 0) {
                hidReport[i] = key;
                modified = true;
                break;
            }
        }
    } else {
        // Remove key from report
        for (int i = 0; i < 6; i++) {
            if (hidReport[i] == key) {
                hidReport[i] = 0;
                modified = true;
                break;
            }
        }
    }

    if (modified) {
        // Send updated HID report
        sendHIDReport(hidReport);
    }
}

void sendHIDReport(uint8_t* report) {
      // Ensure HID interface is active
    customHID.sendReport(report);  // Send the updated report
}