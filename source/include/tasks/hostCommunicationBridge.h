#pragma once
#include <Arduino.h>
#include <USB.h>
#include <USBHIDConsumerControl.h>
#include <USBHIDKeyboard.h>

enum MessageType {
    VOLUME_CHANGE,
    VOLUME_MUTE,
    KEY_PRESS,
    KEY_RELEASE
};

struct HostMessage {
    MessageType type;
    int8_t data;
};

extern QueueHandle_t hostMessageQueue;
extern USBHIDKeyboard Keyboard;
extern USBHIDConsumerControl ConsumerControl;

void hostCommunicationBridge(void* parameters);