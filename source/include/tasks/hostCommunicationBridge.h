#pragma once
#include <Arduino.h>
#include <USBHIDConsumerControl.h>

enum MessageType {
    VOLUME_CHANGE,
    VOLUME_MUTE
};

struct HostMessage {
    MessageType type;
    int8_t data;
};

extern QueueHandle_t hostMessageQueue;
extern USBHIDConsumerControl ConsumerControl;

void hostCommunicationBridge(void* parameters);