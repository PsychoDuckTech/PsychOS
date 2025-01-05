#ifndef SUBMIT_KEY_PRESS_H
#define SUBMIT_KEY_PRESS_H

#include <stdint.h>  // Provides uint8_t
#include "../lib/usb_hid_keys/usb_hid_keys.h"  // For HID-related functions

void submitKeyPress(uint8_t key, bool pressed);
void sendHIDReport(uint8_t* report);

#endif
