#pragma once
#include "arduino_compat.h"
#include <USB.h>
#include <USBHIDConsumerControl.h>

enum MessageType
{
    VOLUME_CHANGE,
    VOLUME_MUTE,
    KEY_PRESS,
    KEY_RELEASE
};

struct HostMessage
{
    MessageType type;
    int8_t data;
};

extern QueueHandle_t hostMessageQueue;
extern USBHIDConsumerControl ConsumerControl;

void hostCommunicationBridge(void *parameters);

void startHostCommTask(UBaseType_t core = 0, uint32_t stackDepth = 4096, UBaseType_t priority = 2);