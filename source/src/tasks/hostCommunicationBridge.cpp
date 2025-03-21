#include "tasks/hostCommunicationBridge.h"
#include "tasks/commandProcessor.h"
#include <USBHIDKeyboard.h>
#include "tasks/wpmCounter.h"

USBHIDKeyboard Keyboard;

QueueHandle_t hostMessageQueue;
USBHIDConsumerControl ConsumerControl;

void hostCommunicationBridge(void *parameters)
{
    hostMessageQueue = xQueueCreate(50, sizeof(HostMessage)); // Increased to 50
    HostMessage receivedMessage;
    Keyboard.begin();
    ConsumerControl.begin();

    Serial.println("Host Communication Bridge started.");

    for (;;)
    {
        if (xQueueReceive(hostMessageQueue, &receivedMessage, portMAX_DELAY) == pdTRUE)
        {
            switch (receivedMessage.type)
            {
            case VOLUME_CHANGE:
                if (receivedMessage.data > 0)
                {
                    ConsumerControl.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
                    ConsumerControl.release();
                }
                else
                {
                    ConsumerControl.press(CONSUMER_CONTROL_VOLUME_DECREMENT);
                    ConsumerControl.release();
                }
                break;
            case VOLUME_MUTE:
                ConsumerControl.press(CONSUMER_CONTROL_MUTE);
                ConsumerControl.release();
                break;
            case KEY_PRESS:
                WPMCounter::recordKeyPress();
                Serial.print("Sending KEY_PRESS to host: code=");
                Serial.println(receivedMessage.data);
                Keyboard.press(receivedMessage.data);
                break;
            case KEY_RELEASE:
                Serial.print("Sending KEY_RELEASE to host: code=");
                Serial.println(receivedMessage.data);
                Keyboard.release(receivedMessage.data);
                break;
            }
        }
    }
}

void startHostCommTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t hostCommHandle;
    xTaskCreatePinnedToCore(
        hostCommunicationBridge, "Host Communication Bridge", 8192, NULL, priority, &hostCommHandle, core);
}