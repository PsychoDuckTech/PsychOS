#pragma once

#include "USB.h" // Use the built-in USB library
#include "Adafruit_TinyUSB.h" // Include TinyUSB header

Adafruit_USBD_HID usb_hid; // Create an instance of Adafruit_USBD_HID

void submit(int data) {
    uint8_t keycode[6] = {static_cast<uint8_t>(HID_KEY_A + data), 0, 0, 0, 0, 0};
    usb_hid.keyboardReport(0, 0, keycode); // Use the instance to send HID report
}