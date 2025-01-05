#include "usb_hid_keys.h"
#include <Arduino.h>  // For Serial debugging and USB functions
// #include <Adafruit_TinyUSB.h>  // TinyUSB library for USB HID

// Adafruit_USBD_HID usb_hid;

// Define the HID report descriptor for a keyboard
const uint8_t report_descriptor[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection (Application)
    0x05, 0x07,  //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,  //   Usage Minimum (0xE0)
    0x29, 0xE7,  //   Usage Maximum (0xE7)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x01,  //   Logical Maximum (1)
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x08,  //   Report Count (8)
    0x81, 0x02,  //   Input (Data,Var,Abs)
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x08,  //   Report Size (8)
    0x81, 0x01,  //   Input (Cnst,Var,Abs)
    0x95, 0x06,  //   Report Count (6)
    0x75, 0x08,  //   Report Size (8)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x65,  //   Logical Maximum (101)
    0x05, 0x07,  //   Usage Page (Kbrd/Keypad)
    0x19, 0x00,  //   Usage Minimum (0)
    0x29, 0x65,  //   Usage Maximum (101)
    0x81, 0x00,  //   Input (Data,Array)
    0xC0         // End Collection
};

//void CustomHID::begin() {
//    // Configure HID device
//    usb_hid.setPollInterval(1);
//    usb_hid.setReportDescriptor(report_descriptor, sizeof(report_descriptor));
//    usb_hid.begin();
//    TinyUSBDevice.begin();  // Start USB device
//    Serial.println("Custom HID interface initialized");
//}
//
//void CustomHID::sendReport(uint8_t* report) {
//    if (usb_hid.ready()) {
//        usb_hid.sendReport(0, report, 8);  // Send HID report
//        Serial.print("HID report sent: ");
//        for (int i = 0; i < 8; i++) {
//            Serial.print(report[i], HEX);
//            Serial.print(" ");
//        }
//        Serial.println();
//    } else {
//        Serial.println("HID not ready");
//    }
//}

// Instantiate the global HID object
CustomHID customHID;