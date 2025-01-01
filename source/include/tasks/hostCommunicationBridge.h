#pragma once
#include <Arduino.h>
#include <USB.h>
#include <USBHIDConsumerControl.h>
#include "config.h"

enum MessageType {
    VOLUME_CHANGE,
    VOLUME_MUTE,
    KEYSTROKE
};

struct HostMessage {
    MessageType type;
    int8_t data;
    int key;
};

extern QueueHandle_t hostMessageQueue;
extern USBHIDConsumerControl ConsumerControl;

void hostCommunicationBridge(void* parameters);
void sendKeyStroke(uint8_t key);