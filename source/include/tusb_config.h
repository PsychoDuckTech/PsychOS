#pragma once

// TinyUSB Configuration for ESP32-S3

#define CFG_TUSB_MCU OPT_MCU_ESP32S3

// RHPort number used for device
#define BOARD_TUD_RHPORT 0

// RHPort max operational speed
#define BOARD_TUD_MAX_SPEED OPT_MODE_HIGH_SPEED

// Device mode with rhport and speed defined by board.mk
#define CFG_TUD_ENABLED 1

// Configuration
#define CFG_TUD_ENDPOINT0_SIZE 64

// Device descriptor
#define CFG_TUD_HID 2
#define CFG_TUD_CDC 0
#define CFG_TUD_MSC 0
#define CFG_TUD_MIDI 0
#define CFG_TUD_VENDOR 0

// HID buffer size
#define CFG_TUD_HID_EP_BUFSIZE 64
